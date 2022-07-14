#include "camera.h"

#include "scene.h"

namespace sgf {

Camera::Camera() {
	viewport.valueChanged.connect(this, [this](CRecti) { invalidateViews(); });
	zNear.valueChanged.connect(this, [this](float) { invalidateViews(); });
	zFar.valueChanged.connect(this, [this](float) { invalidateViews(); });
}

CVector<CameraView> Camera::getViews() const {
	if (m_viewsDirty) {
		// FIXME
		// m_viewsDirty = false;
		m_views = std::move(validateViews());
	}
	return m_views;
}

} // namespace sgf
