#include "orthographiccamera.h"

namespace sgf {

Vector<CameraView> OrthographicCamera::validateViews() const {

	float yh = 1;
	float ar = float(viewport.value().size().x) / float(viewport.value().size().y);

	auto projMatrix = Mat4f::ortho(-yh * ar, yh * ar, -yh, yh, zNear, zFar);

	return {{projMatrix,matrix()}};
}

} // namespace sgf
