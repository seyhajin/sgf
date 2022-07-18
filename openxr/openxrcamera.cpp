#include "openxrcamera.h"

namespace sgf {

namespace {
Mat4f projectionMatrix(float angleLeft, float angleRight, float angleUp, float angleDown, float zNear, float zFar) {

	float left = std::tan(angleLeft), right = std::tan(angleRight), up = std::tan(angleUp), down = std::tan(angleDown);

	float w = right - left, h = up - down, d = zFar - zNear;

	// clang-format off
	return {2 / w,					0,						0,							0,
			0,						2 / h,					0,							0,
			-(right + left) / w,	-(up + down) / h,		(zFar + zNear) / d, 		1,
			0,						0,						-(zFar * zNear * 2) / d,	0};
	// clang-format on
}

}

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

	Vector<CameraView> views(2);

	for (uint eye = 0; eye < 2; ++eye) {

		auto& view = views[eye];

		auto& eyeState=m_xrSession->eyeStates()[eye];
		auto&fov = eyeState.fovAngles;

		view.projectionMatrix = projectionMatrix(fov[0],fov[1],fov[2],fov[3],zNear,zFar);
		view.cameraMatrix = eyeState.eyePose;
		view.frameBuffer = m_frameBuffers[eye][eyeState.swapchainImage];
	}
	return views;
}

} // namespace sgf
