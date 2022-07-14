#include "openxrcamera.h"

namespace sgf {

OpenXRCamera::OpenXRCamera(OpenXRSession* xrSession) : m_xrSession(xrSession) {

	auto glDevice = static_cast<GLGraphicsDevice*>(graphicsDevice());

	TextureFormat format;
	switch (xrSession->swapchainTextureFormat()) {
	case GL_RGBA16:
		format = TextureFormat::rgba64;
		break;
	case GL_RGBA8:
		format = TextureFormat::rgba32;
		break;
	default:
		panic("OOPS");
		break;
	}

	TextureFlags flags = TextureFlags::linear | TextureFlags::clampST;

	auto size = xrSession->swapchainTextureSize();

	// Create a framebuffer for each swapchain image for each eye
	for (uint eye = 0; eye < 2; ++eye) {
		for (auto glTexture : xrSession->swapchainTextures()[eye]) {
			auto texture = glDevice->wrapGLTexture(size.x, size.y, format, flags, glTexture);
			auto frameBuffer = glDevice->createFrameBuffer(texture, nullptr);
			m_frameBuffers[eye].push_back(frameBuffer);
		}
	}

	viewport = Recti(Vec2i(0), size);
}

Vector<CameraView> OpenXRCamera::validateViews() const {

	m_xrSession->updateProjectionMatrices(zNear, zFar);

	Vector<CameraView> views(2);

	for (uint eye = 0; eye < 2; ++eye) {

		auto& view = views[eye];

		view.projectionMatrix = m_xrSession->projectionMatrices()[eye];
		view.cameraMatrix = m_xrSession->eyePoses()[eye];
		view.frameBuffer = m_frameBuffers[eye][m_xrSession->activeSwapchainImages()[eye]];
	}

	return views;
}

} // namespace sgf
