#include "perspectivecamera.h"

#include "scene.h"

#include <window/window.h>

namespace sgf {

PerspectiveCamera::PerspectiveCamera(Scene* scene) : Camera(scene) {
	scene->graphicsDevice()->window->sizeChanged.connect(this, [this](CVec2i size) {debug() << "### sizeChanged"<<size;invalidateViews(); });
	fovY.valueChanged.connect(this, [this](float) { invalidateViews(); });
}

Vector<CameraView> PerspectiveCamera::validateViews() const {

	Recti viewport{0, scene()->graphicsDevice()->window->size()};

	static Recti g_viewport;
	if(viewport!=g_viewport) {
		debug() << "### ARSE" << viewport;
		g_viewport=viewport;
	}

	float yh = std::tan(fovY * degreesToRadians) * zNear;
	float ar = float(viewport.width()) / float(viewport.height());
	auto projMatrix = Mat4f::frustum(-yh * ar, yh * ar, -yh, yh, zNear, zFar);

	return {{viewport, projMatrix, worldMatrix()}};
}

} // namespace sgf
