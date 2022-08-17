#pragma once

#include <scene/camera.h>

namespace sgf {

class XRFrame;

class XRCamera : public Camera {
public:
	SGF_OBJECT_TYPE(XRCamera, Camera);

	Property<float> fovY{45.0f};

	XRCamera(Scene* scene) : Camera(scene) {
	}

	void beginFrame(XRFrame* frame);

	void endFrame();

private:
	XRFrame* m_frame{};

	Vector<CameraView> validateViews() const override;
};

} // namespace sgf
