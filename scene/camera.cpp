#include "camera.h"

#include "scene.h"

namespace sgf {

Camera::Camera(Scene* scene) : Entity(scene) {
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

void Camera::onEnable() {
	scene()->addCamera(this);
}

void Camera::onDisable() {
	scene()->removeCamera(this);
}

void Camera::onUpdate() {
	Super::onUpdate();
	invalidateViews();
}

} // namespace sgf
