#include "collider.h"

#include "scene.h"

namespace sgf {

void Collider::onEnable() {
	scene()->addCollider(this);
}

void Collider::onDisable() {
	scene()->removeCollider(this);
}

void Collider::onUpdate() {
	scene()->updateCollider(this);

}

} // namespace sgf
