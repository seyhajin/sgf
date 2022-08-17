#pragma once

#include "xrsystem.h"

#include <window/window.hh>

#ifdef OS_WINDOWS
#include <windows.h>
#undef min
#undef max
#define XR_USE_PLATFORM_WIN32 1
#define XR_USE_GRAPHICS_API_OPENGL 1
#else
#define XR_USE_PLATFORM_XLIB 1
#define XR_USE_GRAPHICS_API_OPENGL 1
#endif

#include <openxr/openxr_platform.h>

namespace sgf {

class OpenXRFrame;
class OpenXRSession;
class OpenXRSystem;

class OpenXRFrame : public XRFrame {
public:
	OpenXRFrame(OpenXRSession* session);

	const XRViewerPose* getViewerPose() override;

	const XRHandPose* getHandPoses() override;

private:
	OpenXRSession* m_session;
};

class OpenXRSession : public XRSession {
public:
	OpenXRSession(OpenXRSystem* system, Window* window, XrInstance instance);

	void requestFrame(XRFrameFunc func) override;

	FrameBuffer* frameBuffer() override;

	bool create();

private:
	friend class OpenXRFrame;

	Window* m_window{};
	XrInstance m_instance{};
	XrSystemId m_systemId{};
	XrSystemProperties m_systemProperties{};
	XrSession m_session{};
	XrSpace m_localSpace{};
	Vec2i m_swapchainTextureSize{};
	GLenum m_swapchainTextureFormat{};
	Vector<GLuint> m_swapchainTextures{};
	XrSwapchain m_swapchain{};
	Vector<XrSwapchainImageOpenGLKHR> m_swapchainImages;
	XrCompositionLayerProjectionView m_projViews[2]{};
	Recti m_viewports[2];
	uint m_swapchainImage{};
	XrViewLocateInfo m_viewLocateInfo = {XR_TYPE_VIEW_LOCATE_INFO};
	XrCompositionLayerProjection m_projLayer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
	XrCompositionLayerBaseHeader* m_layers[1]{};
	XrFrameEndInfo m_frameEndInfo{XR_TYPE_FRAME_END_INFO};

	XrActionSet m_actionSet{};
	XrAction m_aimPoseAction{};
	XrAction m_gripPoseAction{};
	XrSpace m_aimPoseSpaces[2]{};
	XrSpace m_gripPoseSpaces[2]{};
	XrPosef m_aimPoses[2]{};
	XrPosef m_gripPoses[2]{};
	XrActiveActionSet m_activeActionSets = {{}, XR_NULL_PATH};
	XrActionsSyncInfo m_actionsSyncInfo = {XR_TYPE_ACTIONS_SYNC_INFO, nullptr, 1, &m_activeActionSets};

	bool m_ready = false;
	bool m_rendering = false;
	XrFrameState m_frameState{XR_TYPE_FRAME_STATE};
	XRFrameFunc m_renderFunc{};
	SharedPtr<GLFrameBuffer> m_frameBuffer;
	XRViewerPose m_viewerPose{};
	XRHandPose m_handPoses[2]{};

	void pollEvents();
};

class OpenXRSystem : public XRSystem {
public:
	OpenXRSystem(GraphicsDevice* graphicsDevice) : XRSystem(graphicsDevice){}

	Promise<XRSession*> requestSession() override;

private:
	friend class OpenXRSession;

	XrInstance m_instance{};
};

} // namespace sgf
