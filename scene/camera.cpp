#include "camera.h"

#include "scene.h"

#include <window/window.hh>

namespace sgf {

Camera::Camera() {
	zNear.valueChanged.connect(this, [this](float) { invalidateViews(); });
	zFar.valueChanged.connect(this, [this](float) { invalidateViews(); });
}

CVector<CameraView> Camera::views() const {
	if (m_viewsDirty) {
		m_viewsDirty = false;
		m_views = std::move(validateViews());
	}
	return m_views;
}

void Camera::onUpdate() {
	Super::onUpdate();
	invalidateViews();
}

} // namespace sgf
