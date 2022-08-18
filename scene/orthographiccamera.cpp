#include "orthographiccamera.h"

#include "scene.h"

#include <window/window.h>

namespace sgf {

OrthographicCamera::OrthographicCamera(Scene* scene) : Camera(scene) {
	scene->graphicsDevice->window->sizeChanged.connect(this, [this](CVec2i) { invalidateViews(); });
}

Vector<CameraView> OrthographicCamera::validateViews() const {

	Recti viewport{0, scene()->graphicsDevice->window->size()};

	float ar = float(viewport.width()) / float(viewport.height());
	auto projMatrix = Mat4f::ortho(-ar, ar, -1, 1, zNear, zFar);

	return {{viewport, projMatrix, worldMatrix()}};
}

} // namespace sgf
