#include "perspectivecamera.h"

namespace sgf {

PerspectiveCamera::PerspectiveCamera() {
	fovY.valueChanged.connect(this,[this](float) {invalidateProjectionMatrix();});
}

Mat4f PerspectiveCamera::getProjectionMatrix() const {
	float yh = std::tan(fovY * degreesToRadians) * zNear;
	float ar = float(viewport.value().size().x) / float(viewport.value().size().y);
	return Mat4f::frustum(-yh * ar, yh * ar, -yh, yh, zNear, zFar);
}

} // namespace sgf
