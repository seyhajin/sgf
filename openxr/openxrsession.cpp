#include "openxrsession.h"

// inline static void XrMatrix4x4f_CreateProjection(XrMatrix4x4f* result, GraphicsAPI graphicsApi, const float
// tanAngleLeft, 												 const float tanAngleRight, const float tanAngleUp,
// float const tanAngleDown, const float nearZ, const float farZ) {

#include <glfw/glfw.hh>

#include <glwindow/glwindow.hh>

#include <openxr/xr_linear.h>

#define xrAssert(X) verify((X) == 0);

#define xrInsert(Vec, Num, Type) Vec.insert(Vec.end(), Num, {Type});

namespace sgf {

OpenXRSession::OpenXRSession(GLWindow* window) : m_window(window) {

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

		xrAssert(xrCreateInstance(&info, &m_state.instance));
	}

	// get systemId/systemProperties
	{
		XrSystemGetInfo info{XR_TYPE_SYSTEM_GET_INFO};

		info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

		xrAssert(xrGetSystem(m_state.instance, &info, &m_state.systemId));

		xrAssert(xrGetSystemProperties(m_state.instance, m_state.systemId, &m_state.systemProperties));

		debug() << "### System properties:" << m_state.systemProperties.systemName
				<< "width:" << m_state.systemProperties.graphicsProperties.maxSwapchainImageWidth
				<< "Height:" << m_state.systemProperties.graphicsProperties.maxSwapchainImageHeight << "Layers"
				<< m_state.systemProperties.graphicsProperties.maxLayerCount;
	}

	// Get view configuration views
	{
		m_state.viewConfigurationViews[0].type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
		m_state.viewConfigurationViews[1].type = XR_TYPE_VIEW_CONFIGURATION_VIEW;

		uint n;
		xrAssert(xrEnumerateViewConfigurationViews(m_state.instance, m_state.systemId,
												   XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 2, &n,
												   m_state.viewConfigurationViews));
		assert(n == 2);
		assert(m_state.viewConfigurationViews[0].recommendedImageRectWidth ==
			   m_state.viewConfigurationViews[1].recommendedImageRectWidth);
		assert(m_state.viewConfigurationViews[0].recommendedImageRectHeight ==
			   m_state.viewConfigurationViews[1].recommendedImageRectHeight);

		m_swapchainTextureSize = {int(m_state.viewConfigurationViews[0].recommendedImageRectWidth),
								  int(m_state.viewConfigurationViews[0].recommendedImageRectHeight)};
	}

	// Create session
	{
		PFN_xrGetOpenGLGraphicsRequirementsKHR xrGetOpenGLGraphicsRequirementsKHR = nullptr;
		xrAssert(xrGetInstanceProcAddr(m_state.instance, "xrGetOpenGLGraphicsRequirementsKHR",
									   (PFN_xrVoidFunction*)(&xrGetOpenGLGraphicsRequirementsKHR)));

		XrGraphicsRequirementsOpenGLKHR requirements{XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR};
		xrAssert(xrGetOpenGLGraphicsRequirementsKHR(m_state.instance, m_state.systemId, &requirements));

		debug() << "### Requirements max gl version:" << XR_VERSION_MAJOR(requirements.maxApiVersionSupported) << "."
				<< XR_VERSION_MINOR(requirements.maxApiVersionSupported);
		debug() << "### Requirements min gl version:" << XR_VERSION_MAJOR(requirements.minApiVersionSupported) << "."
				<< XR_VERSION_MINOR(requirements.minApiVersionSupported);

#ifdef OS_WINDOWS
		XrGraphicsBindingOpenGLWin32KHR binding{XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR};
		binding.hDC = GetDC(glfwGetWin32Window(window->glfwWindow()));
		binding.hGLRC = glfwGetWGLContext(window->glfwWindow());

#elif OS_LINUX
		XrGraphicsBindingOpenGLXlibKHR binding{XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR};
		binding.xDisplay = glfwGetX11Display();
		binding.visualid = 0;		   // ?????
		binding.glxFBConfig = nullptr; // ?????
		binding.glxDrawable = glfwGetGLXWindow(window->glfwWindow());
		binding.glxContext = glfwGetGLXContext(window->glfwWindow());

		uint xid;
		glXQueryDrawable(binding.xDisplay, binding.glxDrawable, GLX_FBCONFIG_ID, &xid);

		int nconfigs;
		auto fbconfigs = glXGetFBConfigs(binding.xDisplay, 0, &nconfigs);
		for (uint i = 0; i < nconfigs; ++i) {
			int id;
			glXGetFBConfigAttrib(binding.xDisplay, fbconfigs[i], GLX_FBCONFIG_ID, &id);
			if (id != xid) continue;
			binding.glxFBConfig = fbconfigs[i];
			break;
		}

		auto visual = glXGetVisualFromFBConfig(binding.xDisplay, binding.glxFBConfig);
		binding.visualid = visual->visualid;
#endif
		XrSessionCreateInfo info{XR_TYPE_SESSION_CREATE_INFO};
		info.next = &binding;
		info.systemId = m_state.systemId;

		xrAssert(xrCreateSession(m_state.instance, &info, &m_state.session));
	}

	// Create view and local space
	{
		XrReferenceSpaceCreateInfo info{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};

		info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
		info.poseInReferenceSpace.orientation.w = 1;
		xrAssert(xrCreateReferenceSpace(m_state.session, &info, &m_state.viewSpace));

		info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
		xrAssert(xrCreateReferenceSpace(m_state.session, &info, &m_state.localSpace));
	}

	// Create swapchain FOR EACH EYE
	{
		uint32_t n;
		xrAssert(xrEnumerateSwapchainFormats(m_state.session, 0, &n, nullptr));

		m_state.swapchainFormats.resize(n);
		xrAssert(xrEnumerateSwapchainFormats(m_state.session, n, &n, m_state.swapchainFormats.data()));

		m_swapchainFormat = m_state.swapchainFormats[0]; // GL_RGBA16;

		for (uint eye = 0; eye < 2; ++eye) {

			XrSwapchainCreateInfo info{XR_TYPE_SWAPCHAIN_CREATE_INFO};
			info.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
			info.format = m_swapchainFormat;
			info.sampleCount = 1;
			info.width = m_swapchainTextureSize.x;
			info.height = m_swapchainTextureSize.y;
			info.faceCount = 1;
			info.arraySize = 1;
			info.mipCount = 1;
			xrAssert(xrCreateSwapchain(m_state.session, &info, &m_state.swapchains[eye]));

			auto& projView = m_state.projViews[eye];
			projView.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
			projView.subImage.swapchain = m_state.swapchains[eye];
			projView.subImage.imageRect.offset = {0, 0};
			projView.subImage.imageRect.extent = {m_swapchainTextureSize.x, m_swapchainTextureSize.y};

			xrAssert(xrEnumerateSwapchainImages(m_state.swapchains[eye], 0, &n, nullptr));
			debug() << "### Swapchain images for eye" << eye << ":" << n;

			xrInsert(m_state.swapchainImages[eye], n, XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR);

			xrAssert(xrEnumerateSwapchainImages(m_state.swapchains[eye], n, &n,
												(XrSwapchainImageBaseHeader*)m_state.swapchainImages[eye].data()));

			for (uint j = 0; j < n; ++j) m_swapchainTextures[eye].push_back(m_state.swapchainImages[eye][j].image);
		}
	}

	{
		m_state.viewLocateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
		m_state.viewLocateInfo.space = m_state.localSpace;

		m_state.projLayer.space = m_state.localSpace;
		m_state.projLayer.viewCount = 2;
		m_state.projLayer.views = m_state.projViews;

		m_state.layers[0] = (XrCompositionLayerBaseHeader*)&m_state.projLayer;

		m_state.frameEndInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
		m_state.frameEndInfo.layerCount = 1;
		m_state.frameEndInfo.layers = m_state.layers;
	}
}

OpenXRSession::~OpenXRSession() {

	for (uint eye = 0; eye < 2; ++eye) {
		if (m_state.swapchains[eye]) xrAssert(xrDestroySwapchain(m_state.swapchains[eye]));
	}

	if (m_state.session) xrAssert(xrDestroySession(m_state.session));

	if (m_state.instance) xrAssert(xrDestroyInstance(m_state.instance));
}

void OpenXRSession::pollEvents() {

	XrEventDataBuffer event{XR_TYPE_EVENT_DATA_BUFFER};

	if (xrPollEvent(m_state.instance, &event) == 0) {
		switch (event.type) {
		case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
			auto ev = reinterpret_cast<XrEventDataSessionStateChanged*>(&event);
			if (ev->state == XR_SESSION_STATE_READY) {
				debug() << "### Session state ready!";
				verify(!m_ready);
				XrSessionBeginInfo info{XR_TYPE_SESSION_BEGIN_INFO};
				info.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
				verify(xrBeginSession(m_state.session, &info) == 0);
				debug() << "### Successfully started session";
				m_ready = true;
			} else if (ev->state == XR_SESSION_STATE_SYNCHRONIZED) {
				debug() << "### Session state synchronized!";
			} else if (ev->state == XR_SESSION_STATE_VISIBLE) {
				debug() << "### Session state visible!";
			} else if (ev->state == XR_SESSION_STATE_FOCUSED) {
				debug() << "### Session state focused!";
			}
		} break;
		}
	}
}

bool OpenXRSession::beginFrame() {

	if (!m_ready) return false;

	// Wait for next frame
	//
	xrAssert(xrWaitFrame(m_state.session, nullptr, &m_state.frameState));

	if (!m_state.frameState.shouldRender) return false;

	xrAssert(xrBeginFrame(m_state.session, nullptr));

	// Acquire swapchain images
	//
	for (uint eye = 0; eye < 2; ++eye) {
		xrAssert(xrAcquireSwapchainImage(m_state.swapchains[eye], nullptr, &m_activeSwapchainImages[eye]));

		// FIXME: Can't block forever
		XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO, nullptr, XR_INFINITE_DURATION};
		xrAssert(xrWaitSwapchainImage(m_state.swapchains[eye], &waitInfo));
	}

	// Update eye poses
	//
	XrViewState viewState{XR_TYPE_VIEW_STATE};
	XrView views[2]{{XR_TYPE_VIEW}, {XR_TYPE_VIEW}};
	uint32_t n;
	m_state.viewLocateInfo.displayTime = m_state.frameState.predictedDisplayTime;
	xrAssert(xrLocateViews(m_state.session, &m_state.viewLocateInfo, &viewState, 2, &n, views));
	assert(n == 2);
	for (uint eye = 0; eye < 2; ++eye) {

		auto& view = views[eye];
		auto& projView = m_state.projViews[eye];

		(Vec4f&)m_eyePoses[eye].fovAngles = (Vec4f&)view.fov.angleLeft;
		projView.fov = views[eye].fov;

		auto flags = viewState.viewStateFlags;

		if (flags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) {
			auto q = view.pose.orientation;
			projView.pose.orientation = q;
			//			q.x = -q.x;
			//			q.y = -q.y;
			//			q.w = -q.w;
			//			q.z = -q.z;
			m_eyePoses[eye].orientation = (Quatf&)q;
		}
		if (flags & XR_VIEW_STATE_POSITION_VALID_BIT) {
			auto v = view.pose.position;
			projView.pose.position = v;
			//			v.z = -v.z;
			m_eyePoses[eye].position = (Vec3f&)v;
		}
	}

	return true;
}

void OpenXRSession::endFrame() {

	assert(m_ready && m_state.frameState.shouldRender);
	m_state.frameState.shouldRender = false;

	// Release swapchain images
	//
	for (uint eye = 0; eye < 2; ++eye) xrAssert(xrReleaseSwapchainImage(m_state.swapchains[eye], nullptr));

	// End frame!
	//
	m_state.frameEndInfo.displayTime = m_state.frameState.predictedDisplayTime;
	auto r = xrEndFrame(m_state.session, &m_state.frameEndInfo);
	if (r) { debug() << "### xrEndFrame failed!" << r; }
}

} // namespace sgf
