#pragma once

#include "entity.h"

#include <core3d/core3d.hh>

namespace sgf {

struct CameraView {
	Mat4f projectionMatrix;
	AffineMat4f cameraMatrix;
	SharedPtr<FrameBuffer> frameBuffer;
};

class Camera : public Entity {
public:
	SGF_OBJECT_TYPE(Camera, Entity);

	Property<Recti> viewport{{0, 0, 640, 480}};
	Property<float> zNear{.1f};
	Property<float> zFar{1000.0f};

	Camera();

	CVector<CameraView> getViews() const;

protected:
	virtual Vector<CameraView> validateViews() const = 0;

	void invalidateViews() {
		m_viewsDirty=true;
	}

private:
	mutable Vector<CameraView> m_views;
	mutable bool m_viewsDirty = true;
};

} // namespace sgf
