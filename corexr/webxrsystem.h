#pragma once

#include "xrsystem.h"

namespace sgf {

class WebXRFrame : public XRFrame {
public:
	const XRViewerPose* getViewerPose() override;
};

class WebXRSession : public XRSession {
public:
	Promise<XRFrame*> requestFrame() override;
};

class WebXRSystem : public XRSystem {
public:
	Promise<bool> isSessionSupported() override;

	Promise<XRSession*> requestSession() override;

private:
};

} // namespace sgf
