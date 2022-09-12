#pragma once

#include "camera.h"

namespace sgf {

class OrthographicCamera : public Camera {
public:
	SGF_OBJECT_TYPE(OrthographicCamera, Camera);

	OrthographicCamera();

private:
	Vector<CameraView> validateViews() const override;
};

} // namespace sgf
