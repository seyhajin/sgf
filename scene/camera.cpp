#include "camera.h"

#include "scene.h"

namespace sgf {

namespace {

constexpr float degsToRads = .0174533f;

}

Camera::Camera() {
	viewport.valueChanged.connect(this, [this](CRecti) { m_projDirty = true; });
	zNear.valueChanged.connect(this, [this](float) { m_projDirty = true; });
	zFar.valueChanged.connect(this, [this](float) { m_projDirty = true; });
}

Mat4f Camera::projectionMatrix() const {
	if (m_projDirty) {
		m_projMatrix = getProjectionMatrix();
		m_projDirty = false;
	}
	return m_projMatrix;
}

} // namespace sgf
