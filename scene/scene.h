#pragma once

#include "camera.h"
#include "debugrenderer.h"
#include "light.h"

#include <core/core.hh>

namespace sgf {

class Scene : public Object{
public:
	SGF_OBJECT_TYPE(Scene, Object);

	Property<Vec4f> clearColor;
	Property<Vec3f> ambientLightColor;
	Property<Vec3f> directionalLightVector;
	Property<Vec3f> directionalLightColor;

	explicit Scene(GraphicsDevice* graphicsDevice);

	GraphicsDevice* graphicsDevice()const{
		return m_graphicsDevice;
	}

	float renderTime() const {
		return m_renderTime;
	}

	void addCamera(Camera* camera);
	void removeCamera(Camera* camera);

	void addLight(Light* light);
	void removeLight(Light* light);

	void addRenderer(Renderer* renderer);
	void removeRenderer(Renderer* renderer);

	void render(CVec2i size);

private:
	GraphicsDevice* m_graphicsDevice;
	SharedPtr<GraphicsContext> m_graphicsContext;
	SharedPtr<GraphicsBuffer> m_cameraParams;
	SharedPtr<GraphicsBuffer> m_sceneParams;

	Vector<RenderPass*> m_renderPasses;
	Vector<Camera*> m_cameras;
	Vector<Light*> m_lights;

	RenderContext m_renderContext;

	float m_renderTime = 0;
};

}; // namespace sgf
