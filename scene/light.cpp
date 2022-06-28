#include "light.h"

#include "scene.h"

namespace sgf {

Light::Light(Entity* parent) : Entity(parent) {
}

void Light::onSetEnabled(bool enabled) {
	if (enabled) {
		scene()->addLight(this);
	} else {
		scene()->removeLight(this);
	}
}

} // namespace sgf
