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

	struct EyePose {
		float fovAngles[4]; //left,right,up,down
		Quatf orientation;
		Vec3f position;
	};

	struct OpenXRState {
		XrSessionState sessionState = XR_SESSION_STATE_UNKNOWN;
		XrInstance instance{};
		XrSystemId systemId{};
		XrSystemProperties systemProperties{};
		XrViewConfigurationView viewConfigurationViews[2]{};
		XrSession session{};
		XrSpace viewSpace{};
		XrSpace localSpace{};
		std::vector<int64_t> swapchainFormats{};
		XrSwapchain swapchains[2]{};
		std::vector<XrSwapchainImageOpenGLKHR> swapchainImages[2]{};

		XrFrameState frameState{XR_TYPE_FRAME_STATE};
		XrViewLocateInfo viewLocateInfo = {XR_TYPE_VIEW_LOCATE_INFO};
		XrCompositionLayerProjectionView projViews[2]{};
		XrCompositionLayerProjection projLayer{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
		XrCompositionLayerBaseHeader* layers[1];
		XrFrameEndInfo frameEndInfo{XR_TYPE_FRAME_END_INFO};
	};

	OpenXRSession(GLWindow* window);

	~OpenXRSession();

	GLWindow* window() const {
		return m_window;
	}

	XrSessionState sessionState() const {
		return m_state.sessionState;
	}

	GLenum swapchainFormat() const {
		return m_swapchainFormat;
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

	const EyePose* eyePoses() const {
		return m_eyePoses;
	}

	void pollEvents();

	bool beginFrame();

	void endFrame();

private:
	GLWindow* m_window;
	OpenXRState m_state;
	bool m_ready = false;
	GLenum m_swapchainFormat;
	Vec2i m_swapchainTextureSize;
	Vector<GLuint> m_swapchainTextures[2];
	uint m_activeSwapchainImages[2]{};
	EyePose m_eyePoses[2];
};

} // namespace sgf
