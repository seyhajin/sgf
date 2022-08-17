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
class Collider;
class CollisionSpace;
struct Contact;

class Scene : public Object {
public:
	SGF_OBJECT_TYPE(Scene, Object);

	Property<Vec4f> clearColor;
	Property<Vec3f> ambientLightColor;
	Property<Vec3f> directionalLightVector;
	Property<Vec3f> directionalLightColor;

	Property<SharedPtr<FrameBuffer>> frameBuffer;

	explicit Scene(GraphicsDevice* graphicsDevice);

	GraphicsDevice* graphicsDevice() const {
		return m_graphicsDevice;
	}

	float renderTime() const {
		return m_renderTime;
	}

	bool eyeRay(CVec2f windowCoords, Linef& ray) const;

	Collider* intersectRay(CLinef ray, float radius, Contact& contact) const;

	Collider* intersectEyeRay(CVec2f windowCoords, float radius) const;

	void update();
	void render();

	// ***** INTERNAL *****
	void addRenderer(Renderer* renderer);
	void removeRenderer(Renderer* renderer);

	void addEntity(Entity* entity);
	void removeEntity(Entity* entity);

	void addCamera(Camera* camera);
	void removeCamera(Camera* camera);

	void addLight(Light* light);
	void removeLight(Light* light);

	void addActor(Actor* actor);
	void removeActor(Actor* actor);

	void addCollider(Collider* collider);
	void removeCollider(Collider* collider);
	void updateCollider(Collider* collider);

private:
	GraphicsDevice* m_graphicsDevice;
	SharedPtr<GraphicsContext> m_graphicsContext;
	SharedPtr<GraphicsBuffer> m_cameraParams;
	SharedPtr<GraphicsBuffer> m_sceneParams;

	Vector<RenderPass*> m_renderPasses;
	Vector<Entity*> m_orphans;
	Vector<Camera*> m_cameras;
	Vector<Light*> m_lights;
	Vector<Actor*> m_actors[32];

	RenderContext m_renderContext;

	CollisionSpace* m_collisionSpace;

	float m_renderTime = 0;

	static inline Scene* m_defaultScene;

	friend Scene* defaultScene() {
		return m_defaultScene;
	}

};

}; // namespace sgf
