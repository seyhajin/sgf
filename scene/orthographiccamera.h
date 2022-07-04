#pragma once

#include "camera.h"

namespace sgf {

class OrthographicCamera : public Camera {
public:
	SGF_OBJECT_TYPE(OrthographicCamera, Camera);

private:
	Mat4f getProjectionMatrix() const override;
};

} // namespace sgf
