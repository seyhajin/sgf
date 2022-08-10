#pragma once

#include "camera.h"

namespace sgf {

class PerspectiveCamera : public Camera {
public:
	SGF_OBJECT_TYPE(PerspectiveCamera, Camera);

	Property<float> fovY{45.0f};

	PerspectiveCamera(Scene* scene);

private:
	Vector<CameraView> validateViews() const override;
};

} // namespace sgf
