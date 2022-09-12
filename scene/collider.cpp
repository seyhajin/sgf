#include "collider.h"

#include "collisionspace.h"
#include "scene.h"

namespace sgf {

void Collider::onEnable() {
	Super::onEnable();
	scene->collisionSpace()->addCollider(this);
}

void Collider::onDisable() {
	Super::onDisable();
	scene->collisionSpace()->removeCollider(this);
}

void Collider::onUpdate() {
	Super::onUpdate();
	scene->collisionSpace()->updateCollider(this);
}

} // namespace sgf
