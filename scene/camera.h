#pragma once

#include "entity.h"

namespace sgf {

struct CameraView {
	Recti viewport;
	Mat4f projectionMatrix;
	AffineMat4f cameraMatrix;
};

class Camera : public Entity {
public:
	SGF_OBJECT_TYPE(Camera, Entity);

	Property<float> zNear{.1f};
	Property<float> zFar{100.0f};

	Camera(Scene* scene);

	CVector<CameraView> views() const;

protected:
	virtual Vector<CameraView> validateViews() const = 0;

	void invalidateViews() {
		m_viewsDirty = true;
	}

	void onEnable() override;

	void onDisable() override;

	void onUpdate() override;

private:
	mutable Vector<CameraView> m_views;
	mutable bool m_viewsDirty = true;
};

} // namespace sgf
