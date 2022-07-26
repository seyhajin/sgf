#pragma once

#include "xrsystem.h"

namespace sgf {

class WebXRFrame : public XRFrame {
public:
	WebXRFrame(XRSession* session) : XRFrame(session) {
	}

	const XRViewerPose* getViewerPose() override;

private:
	XRViewerPose m_viewerPose{};
};

class WebXRSession : public XRSession {
public:
	WebXRSession(XRSystem* system) : XRSession(system) {
	}

	void requestFrame(XRFrameFunc func) override;

	FrameBuffer* frameBuffer() override;

private:
	SharedPtr<GLFrameBuffer> m_frameBuffer;
};

class WebXRSystem : public XRSystem {
public:

	Promise<bool> isSessionSupported() override;

	Promise<XRSession*> requestSession() override;

private:
};

extern Signal<> startVRButtonClicked;

void setStartVRButtonEnabled(bool enabled);

} // namespace sgf
