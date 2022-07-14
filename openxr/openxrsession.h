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

	const uint* activeSwapchainImages() const {
		return m_activeSwapchainImages;
	}

	const Mat4f* projectionMatrices() const {
		return m_projMatrices;
	}

	const AffineMat4f* eyePoses() const {
		return m_eyePoses;
	}

	const AffineMat4f* handPoses() const {
		return m_handPoses;
	}

	void pollEvents();

	bool beginFrame();

	void updateProjectionMatrices(float zNear, float zFar);

	void endFrame();

private:
	struct OpenXRState {
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
	OpenXRState m_state;
	bool m_ready = false;
	bool m_rendering = false;
	GLenum m_swapchainTextureFormat{};
	Vec2i m_swapchainTextureSize{};
	Vector<GLuint> m_swapchainTextures[numEyes]{};
	uint m_activeSwapchainImages[numEyes]{};

	Mat4f m_projMatrices[numEyes];
	AffineMat4f m_eyePoses[numEyes];
	AffineMat4f m_handPoses[numHands];
};

} // namespace sgf
