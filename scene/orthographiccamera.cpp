#include "orthographiccamera.h"

namespace sgf {

Mat4f OrthographicCamera::getProjectionMatrix() const {

	float yh = 1;//zNear;
	float ar = float(viewport.value().size().x) / float(viewport.value().size().y);

#if 0
	float yh = std::tan(fovY * degreesToRadians) * zNear;
	float ar = float(viewport.value().size().x) / float(viewport.value().size().y);
	return Mat4f::frustum(-yh * ar, yh * ar, -yh, yh, zNear, zFar);

	float hx = viewport.value().width() / 2;
	float hy = viewport.value().height() / 2;
#endif

	return Mat4f::ortho(-yh * ar, yh * ar, -yh, yh, zNear, zFar);
}

} // namespace sgf
