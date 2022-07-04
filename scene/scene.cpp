#include "scene.h"

#include "shaderloader.h"

namespace sgf {

namespace {

ShaderLoader g_copyShader("shaders/fbcopy.glsl");

} // namespace

Scene::Scene(GraphicsDevice* graphicsDevice) : m_graphicsDevice(graphicsDevice) {

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

	m_cameraParams = m_graphicsDevice->createGraphicsBuffer(BufferType::uniform,sizeof(CameraParams), nullptr);
	m_sceneParams = m_graphicsDevice->createGraphicsBuffer(BufferType::uniform, sizeof(SceneParams), nullptr);

	m_graphicsContext->setUniformBuffer("cameraParams", m_cameraParams);
	m_graphicsContext->setUniformBuffer("sceneParams", m_sceneParams);

	clearColor = Vec4f(.25f, .5f, 1, 1);
	ambientLightColor = Vec3f(.075f);
	directionalLightVector = Vec3f(0.0f, 0.25f, 1.0f).normalized();
	directionalLightColor = Vec3f(.85f);
}

void Scene::addRenderer(Renderer* renderer) {
	for (auto pass : renderer->renderPasses()) { m_renderPasses[int(pass)]->addRenderer(renderer); }
	renderer->attach(this);
}

void Scene::removeRenderer(Renderer* renderer) {
	for (auto pass : renderer->renderPasses()) { m_renderPasses[int(pass)]->removeRenderer(renderer); }
	renderer->detach(this);
}

void Scene::addCamera(Camera* camera) {
	m_cameras.push_back(camera);
}

void Scene::removeCamera(Camera* camera) {
	erase(m_cameras, camera);
}

void Scene::addLight(Light* light) {
	m_lights.push_back(light);
}

void Scene::removeLight(Light* light) {
	erase(m_lights, light);
}

void Scene::render(CVec2i size) {

	float elapsed = 1.0f / 60.0f;
	m_renderTime += elapsed;

	for (RenderPass* p : m_renderPasses) { p->update(); }

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

		rlight.position = Vec4f(light->position(), 1);
		rlight.color = Vec4f(light->color, light->intensity);
		rlight.radius = light->radius;
		rlight.range = light->range;
	}

	m_sceneParams->updateData(0, sizeof(SceneParams), &rscene);

	auto gc = m_graphicsContext;
	m_renderContext.beginScene(gc, &renderParams, size.x, size.y);

	for (auto camera : m_cameras) {

		auto& viewport = camera->viewport.value();

		rcamera.projMatrix = camera->projectionMatrix();
		rcamera.invProjMatrix = rcamera.projMatrix.inverse();
		rcamera.cameraMatrix = camera->matrix();
		rcamera.viewMatrix = rcamera.cameraMatrix.inverse();
		rcamera.viewProjMatrix = rcamera.projMatrix * rcamera.viewMatrix;
		rcamera.clipNear = camera->zNear;
		rcamera.clipFar = camera->zFar;

		rcamera.directionalLightVector = rcamera.viewMatrix * rscene.directionalLightVector;

		for (uint i = 0; i < rscene.numLights; ++i) {
			rcamera.lightPositions[i] = rcamera.viewMatrix * rscene.lights[i].position;
		}

		m_cameraParams->updateData(0, sizeof(CameraParams), &rcamera);

		gc->setViewport(viewport);

		gc->clear(rscene.clearColor);

		for (RenderPass* p : m_renderPasses) p->render(m_renderContext);
	}

	auto sourceTexture = m_renderContext.endScene();

	m_graphicsContext->setFrameBuffer(nullptr);
	m_graphicsContext->setDepthMode(DepthMode::disable);
	m_graphicsContext->setBlendMode(BlendMode::disable);
	m_graphicsContext->setCullMode(CullMode::disable);
	m_graphicsContext->setTexture("sourceTexture", sourceTexture);
	m_graphicsContext->setShader(g_copyShader.open());

	m_graphicsContext->drawGeometry(3, 0, 6, 1);
}

} // namespace sgf
