#include "perspectivecamera.h"

#include "scene.h"

#include <glwindow/glwindow.h>

namespace sgf {

PerspectiveCamera::PerspectiveCamera(Scene* scene) : Camera(scene) {
	scene->graphicsDevice()->window->sizeChanged.connect(this, [this](CVec2i) { invalidateViews(); });
	fovY.valueChanged.connect(this, [this](float) { invalidateViews(); });
}

Vector<CameraView> PerspectiveCamera::validateViews() const {

	Recti viewport{0, scene()->graphicsDevice()->window->size()};

	float yh = std::tan(fovY * degreesToRadians) * zNear;
	float ar = float(viewport.width()) / float(viewport.height());
	auto projMatrix = Mat4f::frustum(-yh * ar, yh * ar, -yh, yh, zNear, zFar);

	return {{viewport, projMatrix, worldMatrix()}};
}

} // namespace sgf
