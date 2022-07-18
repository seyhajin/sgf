#pragma once

#include <geom/geom.hh>
#include <opengl/opengl.hh>

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

class GLWindow;

class OpenXRSession {
public:
	static constexpr uint numEyes = 2;
	static constexpr uint numHands = 2;

	struct EyeState {
		AffineMat4f eyePose;
		float fovAngles[4];
		uint swapchainImage;
	};

	struct ControllerState {
		AffineMat4f gripPose;
		AffineMat4f aimPose;
		bool selectClick;
		bool menuClick;
	};

	OpenXRSession(GLWindow* window);

	~OpenXRSession();

	GLWindow* window() const {
		return m_window;
	}

	XrSessionState sessionState() const {
		return m_state.sessionState;
	}

	GLenum swapchainTextureFormat() const {
		return m_swapchainTextureFormat;
	}

	CVec2i swapchainTextureSize() const {
		return m_swapchainTextureSize;
	}

	const Vector<GLuint>* swapchainTextures() const {
		return m_swapchainTextures;
	}

	const EyeState* eyeStates() const {
		return m_eyeStates;
	}

	const ControllerState* controllerStates() const {
		return m_controllerStates;
	}

	void pollEvents();

	bool beginFrame();

	void endFrame();

private:
	struct XRState {
		XrSessionState sessionState = XR_SESSION_STATE_UNKNOWN;
		XrInstance instance{};
		XrSystemId systemId{};
		XrSystemProperties systemProperties{};
		XrViewConfigurationView viewConfigurationViews[numEyes]{};
		XrSession session{};
		XrSpace viewSpace{};
		XrSpace localSpace{};
		XrSwapchain swapchains[numEyes]{};
		std::vector<XrSwapchainImageOpenGLKHR> swapchainImages[numEyes]{};

		XrFrameState frameState{XR_TYPE_FRAME_STATE};
		XrViewLocateInfo viewLocateInfo = {XR_TYPE_VIEW_LOCATE_INFO};
		XrCompositionLayerProjectionView projViews[numEyes]{};
		XrCompositionLayerProjection projLayer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
		XrCompositionLayerBaseHeader* layers[1];
		XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};

		XrActionSet actionSet{};
		XrAction handPoseAction{};
		XrSpace handPoseSpaces[numHands]{};
		XrPosef handPoses[numHands]{};
		XrActiveActionSet activeActionSets={{},XR_NULL_PATH};
		XrActionsSyncInfo actionsSyncInfo={XR_TYPE_ACTIONS_SYNC_INFO,nullptr,1,&activeActionSets};
	};

	GLWindow* m_window;
	XRState m_state;
	GLenum m_swapchainTextureFormat{};
	Vec2i m_swapchainTextureSize{};
	Vector<GLuint> m_swapchainTextures[numEyes]{};

	EyeState m_eyeStates[numEyes];
	ControllerState m_controllerStates[numHands];

	bool m_ready = false;
	bool m_rendering = false;
};

} // namespace sgf
