#include "actor.h"

#include "scene.h"

namespace sgf {

void Actor::create() {
	assert(m_state == ActorState::init);

	m_state = ActorState::creating;

	enable();

	scene->addEntity(this);

	onCreate();

	m_state = ActorState::active;
}

void Actor::destroy() {
	assert(m_state == ActorState::active);

	m_state = ActorState::destroying;

	scene->removeEntity(this);

	onDestroy();

	disable();

	m_state = ActorState::destroyed;
}

} // namespace sgf
