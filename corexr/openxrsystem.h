#pragma once

#include "xrsystem.h"

namespace sgf {

class OpenXRFrame : public XRFrame{
public:

	const XRViewerPose* getViewerPose() override;
};

class OpenXRSession : public XRSession{
public:

	void requestFrame(XRFrameFunc func) override;

	FrameBuffer* frameBuffer() override;
};

class OpenXRSystem : public XRSystem{
public:

	Promise<XRSession*> requestSession() override;
};

}
