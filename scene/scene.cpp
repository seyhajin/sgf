#include "scene.h"

#include "actor.h"
#include "camera.h"
#include "debugrenderer.h"
#include "light.h"
#include "linearspace.h"
#include "shaderasset.h"

#include <imgui/imgui.hh>

#include <window/window.h>

namespace sgf {

namespace {

ShaderAsset g_copyShader("shaders/fbcopy.glsl");

} // namespace

Scene::Scene(GraphicsDevice* device) : graphicsDevice(device), window(device->window) {

	if (!g_defaultScene) g_defaultScene = this;

	m_renderPasses.resize(numRenderPassTypes);

	m_renderPasses[int(RenderPassType::background)] =
		new RenderPass(RenderPassType::background, DepthMode::disable, BlendMode::disable, CullMode::disable);

	m_renderPasses[int(RenderPassType::opaque)] =
		new RenderPass(RenderPassType::opaque, DepthMode::enable, BlendMode::disable, CullMode::back);

	m_renderPasses[int(RenderPassType::blended)] =
		new RenderPass(RenderPassType::blended, DepthMode::compare, BlendMode::alpha, CullMode::back);

	m_renderPasses[int(RenderPassType::shadows)] =
		new RenderPass(RenderPassType::shadows, DepthMode::compare, BlendMode::multiply, CullMode::back);

	m_renderPasses[int(RenderPassType::sprites)] =
		new RenderPass(RenderPassType::sprites, DepthMode::compare, BlendMode::alpha, CullMode::back);

	m_renderPasses[int(RenderPassType::postfx)] =
		new RenderPass(RenderPassType::postfx, DepthMode::disable, BlendMode::disable, CullMode::back);

	m_renderPasses[int(RenderPassType::overlay)] =
		new RenderPass(RenderPassType::overlay, DepthMode::compare, BlendMode::alpha, CullMode::back);

	m_graphicsContext = graphicsDevice->createGraphicsContext();

	m_cameraParams = graphicsDevice->createGraphicsBuffer(BufferType::uniform, sizeof(CameraParams), nullptr);
	m_sceneParams = graphicsDevice->createGraphicsBuffer(BufferType::uniform, sizeof(SceneParams), nullptr);

	m_graphicsContext->setUniformBuffer("cameraParams", m_cameraParams);
	m_graphicsContext->setUniformBuffer("sceneParams", m_sceneParams);

	clearColor = Vec4f(.25f, .5f, 1, 1);
	ambientLightColor = Vec3f(.25f);
	directionalLightVector = Vec3f(0, 0, 1);
	directionalLightColor = Vec3f(0);

	m_collisionSpace = new LinearSpace();
}

void Scene::addRenderer(Renderer* renderer) {
	for (auto pass : renderer->renderPasses()) { m_renderPasses[int(pass)]->addRenderer(renderer); }
	renderer->attach(this);
}

void Scene::removeRenderer(Renderer* renderer) {
	for (auto pass : renderer->renderPasses()) { m_renderPasses[int(pass)]->removeRenderer(renderer); }
	renderer->detach(this);
}

void Scene::addEntity(Entity* entity) {

	if (auto camera = entity->cast<Camera>()) {
		m_cameras.push_back(camera);
	} else if (auto light = entity->cast<Light>()) {
		m_lights.push_back(light);
	}

	if (entity->parent()) return;

	if (auto actor = entity->cast<Actor>()) {
		auto typeId = actor->dynamicType()->typeId;
		m_actors[typeId].push_back(actor);
	} else {
		m_orphans.push_back(entity);
	}
}

void Scene::removeEntity(Entity* entity) {

	if (auto camera = entity->cast<Camera>()) {
		remove(m_cameras, camera);
	} else if (auto light = entity->cast<Light>()) {
		remove(m_lights, light);
	}

	if (entity->parent()) return;

	if (auto actor = entity->cast<Actor>()) {
		auto typeId = actor->dynamicType()->typeId;
		remove(m_actors[typeId], actor);
	} else {
		remove(m_orphans, entity);
	}
}

bool Scene::eyeRay(CVec2f coords, Linef& ray) const {

	for (auto camera : m_cameras) {
		for (auto& view : camera->views()) {
			if (!view.viewport.contains(coords)) continue;

			Vec2f tcoords = coords - view.viewport.origin();
			tcoords = tcoords / view.viewport.size() * 2 - 1;
			tcoords.y = -tcoords.y;

			auto tv = view.projectionMatrix.inverse() * Vec4f(tcoords, 0, 1);

			auto vv = tv.xyz() / tv.w;
			auto org = vv * (camera->zNear / vv.z);
			auto dst = vv * (camera->zFar / vv.z);

			ray = view.cameraMatrix * Linef(org, dst - org);
			return true;
		}
	}
	return false;
}

Collider* Scene::intersectRay(CLinef ray, float radius, Contact& contact, const Collider* ignore) const {

	return m_collisionSpace->intersectRay(ray, radius, contact, ignore);
}

Collider* Scene::intersectEyeRay(CVec2f coords, float radius) const {

	Linef ray;
	if (!eyeRay(coords, ray)) return nullptr;

	Contact contact;
	contact.time = ray.d.length();
	ray.d.normalize();

	return intersectRay(ray, radius, contact, nullptr);
}

namespace {

void debugEntity(Entity* entity, CString indent = {}) {
	ImGuiEx::DebugRow() << indent + entity->dynamicType()->name << ('\"' + entity->name() + '"') << entity->worldPosition() << entity->worldRotation();
	for (auto child : entity->children()) { debugEntity(child, indent + " "); }
}

} // namespace

void Scene::debugEntities() {

	if(!ImGui::Begin("Scene")) {
		ImGui::End();
		return;
	}

	ImGui::BeginTable("Entities",4);

	for (auto& actors : m_actors) {
		for (auto actor : actors) debugEntity(actor);
	}
	for (auto entity : m_orphans) debugEntity(entity);

	ImGui::EndTable();

	ImGui::End();
}

void Scene::update() {

	for (auto& actors : m_actors) {
		for (auto actor : actors) { actor->update(); }
	}

	for (auto orphan : m_orphans) { orphan->update(); }
}

void Scene::render() {

	float elapsed = 1.0f / 60.0f;
	m_renderTime += elapsed;

	for (RenderPass* p : m_renderPasses) { p->update(); }

	Vec2i size{};
	for (auto camera : m_cameras) {
		for (auto& view : camera->views()) {
			size.x = std::max(size.x, view.viewport.width());
			size.y = std::max(size.y, view.viewport.height());
		}
	}

	RenderParams renderParams;

	auto& rscene = renderParams.scene;
	auto& rcamera = renderParams.camera;

	rscene.clearColor = clearColor;
	rscene.ambientLightColor = Vec4f(ambientLightColor, 1);
	rscene.directionalLightColor = Vec4f(directionalLightColor, 1);
	rscene.directionalLightVector = Vec4f(directionalLightVector, 0);
	rscene.debugFlags = 0;
	rscene.renderTime = m_renderTime;

	//	if (m_lights.size() >= maxLights) {
	// TODO, multiple lighting passes...
	// debug() << "!!! Too many lights in scene:" << m_lights.size() << "max:" << maxLights;
	//	}

	rscene.numLights = std::min(int(m_lights.size()), maxLights);
	for (uint i = 0; i < m_lights.size(); ++i) {
		auto& rlight = rscene.lights[i];
		auto light = m_lights[i];

		rlight.position = Vec4f(light->worldPosition(), 1);
		rlight.color = Vec4f(light->color, light->intensity);
		rlight.radius = light->radius;
		rlight.range = light->range;
	}

	m_sceneParams->updateData(0, sizeof(SceneParams), &rscene);

	auto gc = m_graphicsContext;

	for (auto camera : m_cameras) {

		rcamera.clipNear = camera->zNear;
		rcamera.clipFar = camera->zFar;

		// Iterate through each camera view, eg: left-eye, right-eye for VRCamera
		//
		for (auto& view : camera->views()) {

			auto viewport = view.viewport;

			rcamera.projMatrix = view.projectionMatrix;
			rcamera.invProjMatrix = rcamera.projMatrix.inverse();
			rcamera.cameraMatrix = view.cameraMatrix;
			rcamera.viewMatrix = rcamera.cameraMatrix.inverse();
			rcamera.viewProjMatrix = rcamera.projMatrix * rcamera.viewMatrix;

			rcamera.directionalLightVector = rcamera.viewMatrix * rscene.directionalLightVector;

			for (int i = 0; i < rscene.numLights; ++i) {
				rcamera.lightPositions[i] = rcamera.viewMatrix * rscene.lights[i].position;
			}

			m_cameraParams->updateData(0, sizeof(CameraParams), &rcamera);

			m_renderContext.beginScene(gc, &renderParams, size.x, size.y);

			gc->setViewport({0, viewport.size()});

			gc->clear(rscene.clearColor);

			for (RenderPass* p : m_renderPasses) p->render(m_renderContext);

			auto sourceTexture = m_renderContext.endScene();

			gc->setFrameBuffer(frameBuffer.value() ? frameBuffer.value() : nullptr);
			gc->setViewport(viewport);

			gc->setDepthMode(DepthMode::disable);
			gc->setBlendMode(BlendMode::disable);
			gc->setCullMode(CullMode::disable);
			gc->setTexture("sourceTexture", sourceTexture);
			gc->setUniform("sourceSize", Vec2f(viewport.size()) / Vec2f(size));

			gc->setShader(g_copyShader.open());

			gc->drawGeometry(3, 0, 6, 1);
		}

		// Little hack to show last rendered eye to window
		if (frameBuffer.value()) {

//			exit(1);

			auto viewport = Recti(0, window->size());

			gc->setFrameBuffer(nullptr);
			gc->setViewport(viewport);

			gc->drawGeometry(3, 0, 6, 1);
		}
	}
}

} // namespace sgf
