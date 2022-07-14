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
}

void convertXr(AffineMat4f& m) {

	// Negate Z components and scale by 1,1,-1...
	//	r.m.i.x= m[0][0] ; r.m.i.y= m[1][0] ; r.m.i.z=-m[2][0]
	//  r.m.j.x= m[0][1] ; r.m.j.y= m[1][1] ; r.m.j.z=-m[2][1]
	//  r.m.k.x=-m[0][2] ; r.m.k.y=-m[1][2] ; r.m.k.z= m[2][2]
	//  r.t.x=   m[0][3] ; r.t.y=   m[1][3] ; r.t.z=  -m[2][3]
	m.m.i.z = -m.m.i.z;
	m.m.j.z = -m.m.j.z;
	m.m.k.x = -m.m.k.x;
	m.m.k.y = -m.m.k.y;
	m.t.z = -m.t.z;
}

AffineMat4f cameraMatrix(Quatf orientation, Vec3f position) {
	auto matrix = AffineMat4f(Mat3f(orientation).transpose(), position);
	convertXr(matrix);
	return matrix;
}

} // namespace

OpenXRCamera::OpenXRCamera(OpenXRSession* xrSession) : m_xrSession(xrSession) {

	auto glDevice = static_cast<GLGraphicsDevice*>(graphicsDevice());

	TextureFormat format;
	switch (xrSession->swapchainFormat()) {
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

	// Create a framebuffer for each swapchain image
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

	auto poses = m_xrSession->eyePoses();
	auto images = m_xrSession->activeSwapchainImages();

	Vector<CameraView> views(2);

	for (uint eye = 0; eye < 2; ++eye) {

		auto& view = views[eye];
		auto& pose=poses[eye];
		auto fov = pose.fovAngles;

		view.projectionMatrix = projectionMatrix(fov[0], fov[1], fov[2], fov[3], zNear, zFar);
		view.cameraMatrix = cameraMatrix(pose.orientation,pose.position);
		view.frameBuffer = m_frameBuffers[eye][images[eye]];
	}

	return views;
}

} // namespace sgf
