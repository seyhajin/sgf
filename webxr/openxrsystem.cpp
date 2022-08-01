#include "openxrsystem.h"

#include <glfw/glfw.hh>

#include <opengl/opengl.hh>

#define xrAssert(X) verify((X) >= 0);

//#define CDEBUG debug() << "###"
#define CDEBUG                                                                                                         \
	if constexpr (false) debug()

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

const XRHandPose* OpenXRFrame::getHandPoses() {

	return m_session->m_handPoses;
}

// ***** OpenXRSession *****

OpenXRSession::OpenXRSession(OpenXRSystem* system, GLWindow* window, XrInstance instance)
	: XRSession(system), m_window(window), m_instance(instance) {

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

		if (xrCreateSession(m_instance, &info, &m_session) < 0) return;
	}

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
		xrAssert(xrEnumerateViewConfigurationViews(m_instance, m_systemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 2,
												   &n, viewConfigurationViews));
		assert(n == 2);
		assert(viewConfigurationViews[0].recommendedImageRectWidth ==
			   viewConfigurationViews[1].recommendedImageRectWidth);
		assert(viewConfigurationViews[0].recommendedImageRectHeight ==
			   viewConfigurationViews[1].recommendedImageRectHeight);

		m_swapchainTextureSize = {int(viewConfigurationViews[0].recommendedImageRectWidth),
								  int(viewConfigurationViews[0].recommendedImageRectHeight)};

		CDEBUG << "Swapchain texture size" << m_swapchainTextureSize;

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

	// Crazy action stuff..
	{
		// Create action set
		//
		auto name = "gameplayactions";
		auto locname = "Gameplay Actions";

		XrActionSetCreateInfo createSetInfo{XR_TYPE_ACTION_SET_CREATE_INFO};
		strncpy_s(createSetInfo.actionSetName, XR_MAX_ACTION_SET_NAME_SIZE, "actionset", XR_MAX_ACTION_SET_NAME_SIZE);
		strncpy_s(createSetInfo.localizedActionSetName, XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE, "ActionSet",
				  XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE);

		xrAssert(xrCreateActionSet(m_instance, &createSetInfo, &m_actionSet));
		m_activeActionSets.actionSet = m_actionSet;

		// Create hand pose actions
		//
		XrPath handPaths[2]{};
		xrAssert(xrStringToPath(m_instance, "/user/hand/left", &handPaths[0]));
		xrAssert(xrStringToPath(m_instance, "/user/hand/right", &handPaths[1]));

		// Create aim pose and grip pose actions
		//
		XrActionCreateInfo createInfo{XR_TYPE_ACTION_CREATE_INFO};
		createInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
		createInfo.countSubactionPaths = 2;
		createInfo.subactionPaths = handPaths;

		strncpy_s(createInfo.actionName, XR_MAX_ACTION_NAME_SIZE, "aimposes", XR_MAX_ACTION_NAME_SIZE);
		strncpy_s(createInfo.localizedActionName, XR_MAX_LOCALIZED_ACTION_NAME_SIZE, "Aim Poses",
				  XR_MAX_LOCALIZED_ACTION_NAME_SIZE);
		xrAssert(xrCreateAction(m_actionSet, &createInfo, &m_aimPoseAction));

		strncpy_s(createInfo.actionName, XR_MAX_ACTION_NAME_SIZE, "gripposes", XR_MAX_ACTION_NAME_SIZE);
		strncpy_s(createInfo.localizedActionName, XR_MAX_LOCALIZED_ACTION_NAME_SIZE, "Grip Poses",
				  XR_MAX_LOCALIZED_ACTION_NAME_SIZE);
		xrAssert(xrCreateAction(m_actionSet, &createInfo, &m_gripPoseAction));

		// Create aim pose spaces and grip pose spaces
		//
		for (uint hand = 0; hand < 2; ++hand) {

			XrActionSpaceCreateInfo spaceInfo{XR_TYPE_ACTION_SPACE_CREATE_INFO};

			spaceInfo.subactionPath = handPaths[hand];
			spaceInfo.poseInActionSpace.orientation.w = 1;

			spaceInfo.action = m_aimPoseAction;
			xrAssert(xrCreateActionSpace(m_session, &spaceInfo, &m_aimPoseSpaces[hand]));

			spaceInfo.action = m_gripPoseAction;
			xrAssert(xrCreateActionSpace(m_session, &spaceInfo, &m_gripPoseSpaces[hand]));
		}

		// Associate action with poses
		//
		XrPath profilePath;
		xrAssert(xrStringToPath(m_instance, "/interaction_profiles/khr/simple_controller", &profilePath));

		XrPath bindingPaths[4];
		xrAssert(xrStringToPath(m_instance, "/user/hand/left/input/aim/pose", &bindingPaths[0]));
		xrAssert(xrStringToPath(m_instance, "/user/hand/right/input/aim/pose", &bindingPaths[1]));
		xrAssert(xrStringToPath(m_instance, "/user/hand/left/input/grip/pose", &bindingPaths[2]));
		xrAssert(xrStringToPath(m_instance, "/user/hand/right/input/grip/pose", &bindingPaths[3]));

		// clang-format off
		XrActionSuggestedBinding suggestedBindings[] = {
		{m_aimPoseAction,bindingPaths[0]},
		{m_aimPoseAction,bindingPaths[1]},
		{m_gripPoseAction,bindingPaths[2]},
		{m_gripPoseAction,bindingPaths[3]}};
		// clang-format on

		XrInteractionProfileSuggestedBinding profileBindings = {XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
		profileBindings.interactionProfile = profilePath;
		profileBindings.countSuggestedBindings = std::size(suggestedBindings);
		profileBindings.suggestedBindings = suggestedBindings;

		xrAssert(xrSuggestInteractionProfileBindings(m_instance, &profileBindings));

		// Attach to session
		//
		XrSessionActionSetsAttachInfo attachInfo = {XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO, nullptr, 1, &m_actionSet};
		xrAssert(xrAttachSessionActionSets(m_session, &attachInfo));
	}

	m_valid = true;
}

void OpenXRSession::pollEvents() {

	XrEventDataBuffer event{XR_TYPE_EVENT_DATA_BUFFER};

	if (xrPollEvent(m_instance, &event) == 0) {
		switch (event.type) {
		case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
			auto ev = reinterpret_cast<XrEventDataSessionStateChanged*>(&event);
			if (ev->state == XR_SESSION_STATE_READY) {
				if (m_ready) panic("OOPS");
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

	if (m_rendering) {
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

	// Update controller states
	//
	xrAssert(xrSyncActions(m_session, &m_actionsSyncInfo));

	for (uint hand = 0; hand < 2; ++hand) {

		// Update aim pose
		{
			XrSpaceLocation location{XR_TYPE_SPACE_LOCATION};
			xrAssert(xrLocateSpace(m_aimPoseSpaces[hand], m_localSpace, m_frameState.predictedDisplayTime, &location));

			auto flags = location.locationFlags;

			if (flags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT)
				m_aimPoses[hand].orientation = location.pose.orientation;
			if (flags & XR_SPACE_LOCATION_POSITION_VALID_BIT) m_aimPoses[hand].position = location.pose.position;

			m_handPoses[hand].transform = poseMatrix(m_aimPoses[hand]);
		}

#if 0
		// Update grip pose
		{
			XrSpaceLocation location{XR_TYPE_SPACE_LOCATION};
			xrAssert(xrLocateSpace(m_gripPoseSpaces[hand], m_localSpace,
								   m_frameState.predictedDisplayTime, &location));

			auto flags = location.locationFlags;

			if (flags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT)
				m_gripPoses[hand].orientation = location.pose.orientation;
			if (flags & XR_SPACE_LOCATION_POSITION_VALID_BIT) m_gripPoses[hand].position = location.pose.position;

			m_controllerStates[hand].gripPose = poseMatrix(m_gripPoses[hand]);
		}
#endif
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

		if (m_renderFunc) requestFrame(m_renderFunc);
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

	if (!m_instance) {
		const char* exts[] = {"XR_KHR_opengl_enable"};
		XrInstanceCreateInfo info{XR_TYPE_INSTANCE_CREATE_INFO};
		strncpy_s(info.applicationInfo.applicationName, XR_MAX_APPLICATION_NAME_SIZE, "SGF App",
				  XR_MAX_APPLICATION_NAME_SIZE);
		info.applicationInfo.applicationVersion = XR_VERSION_1_0;
		strncpy_s(info.applicationInfo.engineName, XR_MAX_ENGINE_NAME_SIZE, "SGF", XR_MAX_ENGINE_NAME_SIZE);
		info.applicationInfo.engineVersion = XR_VERSION_1_0;
		info.applicationInfo.apiVersion = 0x0001000000000000L;
		info.enabledExtensionCount = std::size(exts);
		info.enabledExtensionNames = exts;

		if (xrCreateInstance(&info, &m_instance) < 0) m_instance = nullptr;
	}
	return {m_instance != nullptr};
}

Promise<XRSession*> OpenXRSystem::requestSession() {

	return isSessionSupported() | [this](bool supported) {
		if (supported) {
			m_session = new OpenXRSession(this, m_window, m_instance);
			if (!m_session->valid()) {
				delete m_session;
				m_session = nullptr;
			}
		}
		return Promise<XRSession*>(m_session);
	};
}

} // namespace sgf
