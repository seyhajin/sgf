#include "scene.h"

#include "actor.h"
#include "camera.h"
#include "debugrenderer.h"
#include "light.h"
#include "linearspace.h"
#include "shaderloader.h"

#include <window/window.h>

namespace sgf {

namespace {

ShaderLoader g_copyShader("shaders/fbcopy.glsl");

} // namespace

Scene::Scene(GraphicsDevice* graphicsDevice) : m_graphicsDevice(graphicsDevice) {

	if (!m_defaultScene) m_defaultScene = this;

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

	m_graphicsContext = m_graphicsDevice->createGraphicsContext();

	m_cameraParams = m_graphicsDevice->createGraphicsBuffer(BufferType::uniform, sizeof(CameraParams), nullptr);
	m_sceneParams = m_graphicsDevice->createGraphicsBuffer(BufferType::uniform, sizeof(SceneParams), nullptr);

	m_graphicsContext->setUniformBuffer("cameraParams", m_cameraParams);
	m_graphicsContext->setUniformBuffer("sceneParams", m_sceneParams);

	clearColor = Vec4f(.25f, .5f, 1, 1);
	ambientLightColor = Vec3f(.075f);
	directionalLightVector = Vec3f(0.0f, 0.25f, 1.0f).normalized();
	directionalLightColor = Vec3f(.85f);

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
	if (!entity->parent()) m_orphans.push_back(entity);
}

void Scene::removeEntity(Entity* entity) {
	if (!entity->parent()) remove(m_orphans, entity);
}

void Scene::addCamera(Camera* camera) {
	assert(!contains(m_cameras, camera));
	m_cameras.push_back(camera);
	addEntity(camera);
}

void Scene::removeCamera(Camera* camera) {
	assert(contains(m_cameras, camera));
	remove(m_cameras, camera);
	removeEntity(camera);
}

void Scene::addLight(Light* light) {
	assert(!contains(m_lights, light));
	m_lights.push_back(light);
	addEntity(light);
}

void Scene::removeLight(Light* light) {
	assert(contains(m_lights, light));
	remove(m_lights, light);
	removeEntity(light);
}

void Scene::addActor(Actor* actor) {
	auto& actors = m_actors[actor->typeId()];
	assert(!contains(actors, actor));
	assert(!actor->parent());
	actors.push_back(actor);
	actor->create();
}

void Scene::removeActor(Actor* actor) {
	auto& actors = m_actors[actor->typeId()];
	assert(contains(actors, actor));
	remove(actors, actor);
	actor->destroy();
}

void Scene::addCollider(Collider* collider) {
	m_collisionSpace->addCollider(collider);
}

void Scene::removeCollider(Collider* collider) {
	m_collisionSpace->removeCollider(collider);
}

void Scene::updateCollider(Collider* collider) {
	m_collisionSpace->updateCollider(collider);
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

Collider* Scene::intersectRay(CLinef ray, float radius, Contact& contact) const {
	return m_collisionSpace->intersectRay(ray, radius, contact);
}

Collider* Scene::intersectEyeRay(CVec2f coords, float radius) const {

	Linef ray;
	if (!eyeRay(coords, ray)) return nullptr;

	Contact contact;
	contact.time = ray.d.length();
	ray.d.normalize();

	return intersectRay(ray, radius, contact);
}

void Scene::update() {

	// Update actors in typeId order
	for (auto& actors : m_actors) {
		for (auto actor : actors) actor->update();
	}

	// Update orphans
	for (auto orphan : m_orphans) orphan->update();
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
	rscene.debugFlags = 0; // settings::debugTriangles;
	rscene.renderTime = m_renderTime;

	if (m_lights.size() >= maxLights) {
		// TODO, multiple lighting passes...
		// debug() << "!!! Too many lights in scene:" << m_lights.size() << "max:" << maxLights;
	}

	rscene.numLights = std::min((int)m_lights.size(), maxLights);
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

			auto& viewport = view.viewport;

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
			gc->setUniform("viewportSize", Vec2f(viewport.size()) / Vec2f(size));

			gc->setShader(g_copyShader.open());

			gc->drawGeometry(3, 0, 6, 1);
		}
	}
}

} // namespace sgf
