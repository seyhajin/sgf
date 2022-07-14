#pragma once

#include "openxrsession.h"

#include <scene/scene.hh>

namespace sgf{

class OpenXRCamera : public Camera {
public:
	OpenXRCamera(OpenXRSession* xrSession);

private:
	OpenXRSession* m_xrSession;
	Vector<SharedPtr<FrameBuffer>> m_frameBuffers[2];

	Vector<CameraView> validateViews()const override;
};

}
