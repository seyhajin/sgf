#include "actor.h"

#include "scene.h"

namespace sgf {

void Actor::create() {
	assert(m_state == ActorState::init);

	m_state = ActorState::creating;

	scene()->addActor(this);

	onCreate();

	enable();

	m_state = ActorState::active;
}

void Actor::destroy() {
	assert(m_state == ActorState::active);

	m_state = ActorState::destroying;

	disable();

	onDestroy();

	scene()->removeActor(this);

	m_state = ActorState::destroyed;
}

} // namespace sgf
