#include "openxrsystem.h"

#include <glfw/glfw.hh>

#include <opengl/opengl.hh>

#define xrAssert(X) verify((X) >= 0);

//#define CDEBUG debug() << "###"
#define CDEBUG if constexpr(false) debug()

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

AffineMat4f poseMatrix(XrQuaternionf orientation, XrVector3f position) {

	auto m = AffineMat4f(Mat3f((Quatf&)orientation).transpose(), (Vec3f&)position);

	m.m.i.z = -m.m.i.z;
	m.m.j.z = -m.m.j.z;
	m.m.k.x = -m.m.k.x;
	m.m.k.y = -m.m.k.y;
	m.t.z = -m.t.z;

	return m;
}

AffineMat4f poseMatrix(const XrPosef& pose) {

	auto m = AffineMat4f(Mat3f((CQuatf)pose.orientation).transpose(), (CVec3f)pose.position);

	m.m.i.z = -m.m.i.z;
	m.m.j.z = -m.m.j.z;
	m.m.k.x = -m.m.k.x;
	m.m.k.y = -m.m.k.y;
	m.t.z = -m.t.z;

	return m;
}

} // namespace

// **** OpenXRFrame *****

OpenXRFrame::OpenXRFrame(OpenXRSession* session) : XRFrame(session), m_session(session) {
}

const XRViewerPose* OpenXRFrame::getViewerPose() {

	return &m_session->m_viewerPose;
}

// ***** OpenXRSession *****

OpenXRSession::OpenXRSession(OpenXRSystem* system, XrSession session)
	: XRSession(system), m_system(system), m_session(session) {

	// Create local reference space
	{
		XrReferenceSpaceCreateInfo info{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};

		info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
		info.poseInReferenceSpace.orientation.w = 1;
		xrAssert(xrCreateReferenceSpace(m_session, &info, &m_localSpace));
	}

	// Get view configuration views
	{
		XrViewConfigurationView viewConfigurationViews[2]{};

		viewConfigurationViews[0].type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
		viewConfigurationViews[1].type = XR_TYPE_VIEW_CONFIGURATION_VIEW;

		uint n;
		xrAssert(xrEnumerateViewConfigurationViews(m_system->m_instance, m_system->m_systemId,
												   XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 2, &n,
												   viewConfigurationViews));
		assert(n == 2);
		assert(viewConfigurationViews[0].recommendedImageRectWidth ==
			   viewConfigurationViews[1].recommendedImageRectWidth);
		assert(viewConfigurationViews[0].recommendedImageRectHeight ==
			   viewConfigurationViews[1].recommendedImageRectHeight);

		m_swapchainTextureSize = {int(viewConfigurationViews[0].recommendedImageRectWidth),
								  int(viewConfigurationViews[0].recommendedImageRectHeight)};

		CDEBUG << "Swapchain texture size"<<m_swapchainTextureSize;

		m_viewports[0] = Recti(0, 0, m_swapchainTextureSize.y, m_swapchainTextureSize.y);
		m_viewports[1] = Recti(m_swapchainTextureSize.x, 0, m_swapchainTextureSize.x * 2, m_swapchainTextureSize.y);
	}

	// Create swapchain and framebuffer
	{
		uint32_t n;
		xrAssert(xrEnumerateSwapchainFormats(m_session, 0, &n, nullptr));

		Vector<int64_t> swapchainFormats(n);
		xrAssert(xrEnumerateSwapchainFormats(m_session, n, &n, swapchainFormats.data()));

		for (auto format : swapchainFormats) {
			switch (format) {
			case GL_RGBA16:
			case GL_RGBA8:
				m_swapchainTextureFormat = format;
				break;
			default:
				continue;
			}
			break;
		}
		if (!m_swapchainTextureFormat) panic("Can't find suitable swapchain texture format");

		XrSwapchainCreateInfo info{XR_TYPE_SWAPCHAIN_CREATE_INFO};
		info.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
		info.format = m_swapchainTextureFormat;
		info.sampleCount = 1;
		info.width = m_swapchainTextureSize.x * 2;
		info.height = m_swapchainTextureSize.y;
		info.faceCount = 1;
		info.arraySize = 1;
		info.mipCount = 1;
		xrAssert(xrCreateSwapchain(m_session, &info, &m_swapchain));

		xrAssert(xrEnumerateSwapchainImages(m_swapchain, 0, &n, nullptr));
		m_swapchainImages.insert(m_swapchainImages.end(), n, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR});
		xrAssert(xrEnumerateSwapchainImages(m_swapchain, n, &n, (XrSwapchainImageBaseHeader*)m_swapchainImages.data()));

		for (uint i = 0; i < n; ++i) m_swapchainTextures.push_back(m_swapchainImages[i].image);

		GLuint glFramebuffer;
		glGenFramebuffers(1, &glFramebuffer);
		m_frameBuffer = new GLFrameBuffer(graphicsDevice(), nullptr, nullptr, m_swapchainTextureSize.x * 2,
										  m_swapchainTextureSize.y, glFramebuffer);
	}

	// Initialize layers
	{
		m_viewLocateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
		m_viewLocateInfo.space = m_localSpace;

		for (uint eye = 0; eye < 2; ++eye) {
			auto& projView = m_projViews[eye];
			projView.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
			projView.subImage.swapchain = m_swapchain;
			projView.subImage.imageRect.offset = {m_viewports[eye].x(), m_viewports[eye].y()};
			projView.subImage.imageRect.extent = {m_viewports[eye].width(), m_viewports[eye].height()};
		}

		m_projLayer.space = m_localSpace;
		m_projLayer.viewCount = 2;
		m_projLayer.views = m_projViews;

		m_layers[0] = (XrCompositionLayerBaseHeader*)&m_projLayer;

		m_frameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
		m_frameEndInfo.layerCount = 1;
		m_frameEndInfo.layers = m_layers;
	}
}

void OpenXRSession::pollEvents() {

	XrEventDataBuffer event{XR_TYPE_EVENT_DATA_BUFFER};

	if (xrPollEvent(m_system->m_instance, &event) == 0) {
		switch (event.type) {
		case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
			auto ev = reinterpret_cast<XrEventDataSessionStateChanged*>(&event);
			if (ev->state == XR_SESSION_STATE_READY) {
				if(m_ready) panic("OOPS");
				XrSessionBeginInfo info{XR_TYPE_SESSION_BEGIN_INFO};
				info.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
				verify(xrBeginSession(m_session, &info) == 0);
				m_ready = true;
			} else if (ev->state == XR_SESSION_STATE_SYNCHRONIZED) {
			} else if (ev->state == XR_SESSION_STATE_VISIBLE) {
			} else if (ev->state == XR_SESSION_STATE_FOCUSED) {
			}
		} break;
		default:
			break;
		}
	}
}

void OpenXRSession::requestFrame(XRFrameFunc func) {

	if(m_rendering) {
		m_renderFunc = func;
		return;
	}

	pollEvents();

	if (!m_ready) {
		sleep(.1) | [this, func](bool) { requestFrame(func); };
		return;
	}

	// Wait for next frame
	//
	xrAssert(xrWaitFrame(m_session, nullptr, &m_frameState));
	if (!m_frameState.shouldRender) {
		sleep(.1) | [this, func](bool) { requestFrame(func); };
		return;
	} // return false;

	xrAssert(xrBeginFrame(m_session, nullptr));

	xrAssert(xrAcquireSwapchainImage(m_swapchain, nullptr, &m_swapchainImage));

	XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO, nullptr, XR_INFINITE_DURATION};
	xrAssert(xrWaitSwapchainImage(m_swapchain, &waitInfo));

	// Update projviews and eye poses
	//
	XrViewState viewState{XR_TYPE_VIEW_STATE};
	XrView views[2]{{XR_TYPE_VIEW}, {XR_TYPE_VIEW}};
	uint32_t n;
	m_viewLocateInfo.displayTime = m_frameState.predictedDisplayTime;
	xrAssert(xrLocateViews(m_session, &m_viewLocateInfo, &viewState, 2, &n, views));
	assert(n == 2);

	// Update eye states
	//
	for (uint eye = 0; eye < 2; ++eye) {

		auto& view = views[eye];
		auto& projView = m_projViews[eye];
		projView.fov = view.fov;

		auto flags = viewState.viewStateFlags;
		if (flags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) projView.pose.orientation = view.pose.orientation;
		if (flags & XR_VIEW_STATE_POSITION_VALID_BIT) projView.pose.position = view.pose.position;

		float zNear = .1f;
		float zFar = 100.0f;

		auto& fov = projView.fov;

		m_viewerPose.views[eye].projectionMatrix =
			projectionMatrix(fov.angleLeft, fov.angleRight, fov.angleUp, fov.angleDown, zNear, zFar);
		m_viewerPose.views[eye].transform = poseMatrix(projView.pose);
		m_viewerPose.views[eye].viewport = m_viewports[eye];
	}

	postAppEvent([this, func] {
		auto frame = new OpenXRFrame(this);

		auto texture = m_swapchainTextures[m_swapchainImage];

		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->glFramebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		m_renderFunc = {};
		m_rendering = true;

		func(m_frameState.predictedDisplayTime, frame);

		m_rendering = false;

		xrAssert(xrReleaseSwapchainImage(m_swapchain, nullptr));

		// End frame!
		//
		m_frameEndInfo.displayTime = m_frameState.predictedDisplayTime;
		xrAssert(xrEndFrame(m_session, &m_frameEndInfo));

		if(m_renderFunc) requestFrame(m_renderFunc);
	});
}

// ***** OpenXRSession::frameBuffer *****

FrameBuffer* OpenXRSession::frameBuffer() {
	return m_frameBuffer;
}

// ***** OpenXRSystem *****

OpenXRSystem::OpenXRSystem(GLWindow* window) : m_window(window) {
}

Promise<bool> OpenXRSystem::isSessionSupported() {
	return {true};
}

Promise<XRSession*> OpenXRSystem::requestSession() {

	XRSession* xrSession = nullptr;

	// Create instance
	{
		const char* exts[] = {"XR_KHR_opengl_enable"};

		XrInstanceCreateInfo info{XR_TYPE_INSTANCE_CREATE_INFO};

		strncpy_s(info.applicationInfo.applicationName, XR_MAX_APPLICATION_NAME_SIZE, "TestApp",
				  XR_MAX_APPLICATION_NAME_SIZE);
		info.applicationInfo.applicationVersion = XR_VERSION_1_0;
		strncpy_s(info.applicationInfo.engineName, XR_MAX_ENGINE_NAME_SIZE, "SGF", XR_MAX_ENGINE_NAME_SIZE);
		info.applicationInfo.engineVersion = XR_VERSION_1_0;
		info.applicationInfo.apiVersion = 0x0001000000000000L;
		info.enabledExtensionCount = std::size(exts);
		info.enabledExtensionNames = exts;

		xrAssert(xrCreateInstance(&info, &m_instance));
	}

	// get systemId/systemProperties
	{
		XrSystemGetInfo info{XR_TYPE_SYSTEM_GET_INFO};

		info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

		xrAssert(xrGetSystem(m_instance, &info, &m_systemId));

		xrAssert(xrGetSystemProperties(m_instance, m_systemId, &m_systemProperties));

		CDEBUG << "System properties:" << m_systemProperties.systemName
				<< "width:" << m_systemProperties.graphicsProperties.maxSwapchainImageWidth
				<< "Height:" << m_systemProperties.graphicsProperties.maxSwapchainImageHeight << "Layers"
				<< m_systemProperties.graphicsProperties.maxLayerCount;
	}

	// Create session
	{
		PFN_xrGetOpenGLGraphicsRequirementsKHR xrGetOpenGLGraphicsRequirementsKHR = nullptr;
		xrAssert(xrGetInstanceProcAddr(m_instance, "xrGetOpenGLGraphicsRequirementsKHR",
									   (PFN_xrVoidFunction*)(&xrGetOpenGLGraphicsRequirementsKHR)));

		XrGraphicsRequirementsOpenGLKHR requirements{XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR};
		xrAssert(xrGetOpenGLGraphicsRequirementsKHR(m_instance, m_systemId, &requirements));

		CDEBUG << "Requirements - min gl version:" << XR_VERSION_MAJOR(requirements.minApiVersionSupported) << "."
			   << XR_VERSION_MINOR(requirements.minApiVersionSupported);

		CDEBUG << "Requirements -  max gl version:" << XR_VERSION_MAJOR(requirements.maxApiVersionSupported) << "."
				<< XR_VERSION_MINOR(requirements.maxApiVersionSupported);

		XrGraphicsBindingOpenGLWin32KHR binding{XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR};
		binding.hDC = GetDC(glfwGetWin32Window(m_window->glfwWindow()));
		binding.hGLRC = glfwGetWGLContext(m_window->glfwWindow());

		XrSessionCreateInfo info{XR_TYPE_SESSION_CREATE_INFO};
		info.next = &binding;
		info.systemId = m_systemId;

		XrSession session{};
		xrAssert(xrCreateSession(m_instance, &info, &session));

		xrSession = new OpenXRSession(this, session);
	}
	return {xrSession};
}

} // namespace sgf
