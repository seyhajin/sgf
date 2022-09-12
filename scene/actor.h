#pragma once

#include "entity.h"
#include "actortype.h"

namespace sgf {

class Scene;

enum struct ActorState { init, creating, active, destroying, destroyed };

class Actor : public Entity {
public:
	SGF_ACTOR_TYPE(Actor, Entity, 0);

	ActorState state() const {
		return m_state;
	}

	void create();

	void destroy();

protected:
	virtual void onCreate() {
	}

	virtual void onDestroy() {
	}

private:
	ActorState m_state{};
};

} // namespace sgf
