#pragma once

#include "entity.h"

namespace sgf {

class Scene;

enum struct ActorState { init, creating, active, destroying, destroyed };

class ActorType : public ObjectType {
public:
	uint const typeId;

	ActorType(const char* name, ObjectType* super, uint typeId) noexcept : ObjectType(name, super), typeId(typeId) {
	}
};

#define SGF_ACTOR_TYPE(name, super, typeId)                                                                            \
	using Super = super;                                                                                               \
	static inline ActorType staticType{#name, &Super::staticType, typeId};                                             \
	ActorType* dynamicType() const override { return &staticType; }

class Actor : public Entity {
public:
	SGF_ACTOR_TYPE(Actor, Entity, 0);

	Actor(Scene* scene) : Entity(scene) {
	}

	ActorState state() const {
		return m_state;
	}

	uint typeId() const {
		return dynamicType()->typeId;
	}

	void create();

	void destroy();

protected:
	// Invoked when this actor added to scene
	virtual void onCreate() {
	}

	// Invoked when this actor removed from scene
	virtual void onDestroy() {
	}

private:
	ActorState m_state{};
};

} // namespace sgf
