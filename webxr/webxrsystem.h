#pragma once

#include "xrsystem.h"

namespace sgf {

class WebXRFrame : public XRFrame {
public:
	WebXRFrame(XRSession* session) : XRFrame(session) {
	}

	const XRViewerPose* getViewerPose() override;

	const XRHandPose* getHandPoses() override;

private:
	XRViewerPose m_viewerPose{};

	XRHandPose m_handPoses[2]{};
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
	WebXRSystem(GraphicsDevice* graphicsDevice) : XRSystem(graphicsDevice){}

	Promise<XRSession*> requestSession() override;

private:
};

} // namespace sgf
