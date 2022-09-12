#pragma once

#include "camera.h"
#include "debugrenderer.h"
#include "light.h"

#include <core/core.hh>

namespace sgf {

class Entity;
class Actor;
class Camera;
class Light;
class Renderer;
class RigidBody;
class Collider;
class CollisionSpace;
struct Contact;

class Scene : public Object {
public:
	SGF_OBJECT_TYPE(Scene, Object);

	GraphicsDevice* const graphicsDevice;
	Window* const window;

	Property<Vec4f> clearColor;
	Property<Vec3f> ambientLightColor;
	Property<Vec3f> directionalLightVector;
	Property<Vec3f> directionalLightColor;
	Property<SharedPtr<FrameBuffer>> frameBuffer;

	explicit Scene(GraphicsDevice* graphicsDevice);

	CollisionSpace* collisionSpace() const {
		return m_collisionSpace;
	}

	float renderTime() const {
		return m_renderTime;
	}

	bool eyeRay(CVec2f windowCoords, Linef& ray) const;
	Collider* intersectRay(CLinef ray, float radius, Contact& contact, const Collider* ignore) const;
	Collider* intersectEyeRay(CVec2f windowCoords, float radius) const;

	void update();
	void render();

	// ***** DEBUG *****
	void debugEntities();

	// ***** INTERNAL *****
	void addRenderer(Renderer* renderer);
	void removeRenderer(Renderer* renderer);

	void addEntity(Entity* entity);
	void removeEntity(Entity* entity);

private:
	SharedPtr<GraphicsContext> m_graphicsContext;
	SharedPtr<GraphicsBuffer> m_cameraParams;
	SharedPtr<GraphicsBuffer> m_sceneParams;

	Vector<RenderPass*> m_renderPasses;
	Vector<Camera*> m_cameras;
	Vector<Light*> m_lights;
	Vector<Entity*> m_orphans;
	Vector<Actor*> m_actors[32];

	RenderContext m_renderContext;

	CollisionSpace* m_collisionSpace;

	float m_renderTime = 0;

	static inline Scene* g_defaultScene;

	friend Scene* defaultScene() {
		return g_defaultScene;
	}
};

Scene* defaultScene();

}; // namespace sgf
