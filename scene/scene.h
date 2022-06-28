#pragma once

#include "camera.h"
#include "debugrenderer.h"
#include "light.h"

#include <core/core.hh>

namespace sgf {

class Scene : public Object {
public:
	SGF_OBJECT_TYPE(Scene, Object);

	Property<Vec4f> clearColor;
	Property<Vec3f> ambientLightColor;
	Property<Vec3f> directionalLightVector;
	Property<Vec3f> directionalLightColor;

	Scene();

	float renderTime() const {
		return m_renderTime;
	}

	void addCamera(Camera* camera);
	void removeCamera(Camera* camera);

	void addLight(Light* light);
	void removeLight(Light* light);

	void addRenderer(Renderer* renderer);
	void removeRenderer(Renderer* renderer);

	DebugRenderer* debugRenderer();

	void render(CVec2i size);

private:
	Vector<RenderPass*> m_renderPasses;

	SharedPtr<GraphicsContext> m_graphicsContext;
	SharedPtr<UniformBuffer> m_cameraParams;
	SharedPtr<UniformBuffer> m_sceneParams;

	Vector<Camera*> m_cameras;
	Vector<Light*> m_lights;

	RenderContext m_renderContext;

	mutable DebugRenderer* m_debugRenderer = nullptr;
	float m_renderTime = 0;

	static Scene* g_instance;
	friend Scene* activeScene() {
		return g_instance;
	}
};

Scene* activeScene();

}; // namespace sgf
