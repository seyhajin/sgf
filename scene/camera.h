#pragma once

#include "entity.h"

namespace sgf {

class Camera : public Entity {
public:
	SGF_OBJECT_TYPE(Camera, Entity);

	Property<Recti> viewport{{0, 0, 640, 480}};
	Property<float> zNear{.1f};
	Property<float> zFar{1000.0f};

	Camera();

	Mat4f projectionMatrix() const;

protected:
	virtual Mat4f getProjectionMatrix() const = 0;

	void invalidateProjectionMatrix(){
		m_projDirty=true;
	}

private:
	mutable Mat4f m_projMatrix;
	mutable bool m_projDirty = true;

};

} // namespace sgf
