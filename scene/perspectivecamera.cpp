#include "perspectivecamera.h"

namespace sgf {

PerspectiveCamera::PerspectiveCamera() {
	fovY.valueChanged.connect(this,[this](float) {invalidateViews();});
}

Vector<CameraView> PerspectiveCamera::validateViews() const{

	float yh = std::tan(fovY * degreesToRadians) * zNear;
	float ar = float(viewport.value().size().x) / float(viewport.value().size().y);
	auto projMatrix = Mat4f::frustum(-yh * ar, yh * ar, -yh, yh, zNear, zFar);

	return {{projMatrix,matrix()}};
}

} // namespace sgf
