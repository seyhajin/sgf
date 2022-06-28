#include "camera.h"

#include "scene.h"

namespace sgf {

namespace {

constexpr float degsToRads = .0174533f;

}

Camera::Camera(Entity* parent) : Entity(parent) {
	fovY.valueChanged.connect(this, [this](float) { m_projDirty = true; });
	zNear.valueChanged.connect(this, [this](float) { m_projDirty = true; });
	zFar.valueChanged.connect(this, [this](float) { m_projDirty = true; });
	viewport.valueChanged.connect(this, [this](CRecti) { m_projDirty = true; });
}

Mat4f Camera::projectionMatrix() const {
	if (m_projDirty) {
		m_projDirty = false;
		float yh = std::tan(fovY * degsToRads) * zNear;
		float ar = float(viewport.value().size().x) / float(viewport.value().size().y);
		m_projMatrix = Mat4f::frustum(-yh * ar, yh * ar, -yh, yh, zNear, zFar);
	}
	return m_projMatrix;
}

void Camera::onSetEnabled(bool enabled) {
	if (enabled) {
		scene()->addCamera(this);
	} else {
		scene()->removeCamera(this);
	}
}

} // namespace sgf
