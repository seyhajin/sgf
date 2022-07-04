#pragma once

#include "camera.h"

namespace sgf {

class PerspectiveCamera : public Camera {
public:
	SGF_OBJECT_TYPE(PerspectiveCamera, Camera);

	Property<float> fovY{45.0f};

	PerspectiveCamera();

private:
	Mat4f getProjectionMatrix() const override;
};

} // namespace sgf
