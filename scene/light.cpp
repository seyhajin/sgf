#include "light.h"

#include "scene.h"

namespace sgf {

void Light::onEnable() {
	scene()->addLight(this);
}

void Light::onDisable() {
	scene()->removeLight(this);
}

} // namespace sgf
