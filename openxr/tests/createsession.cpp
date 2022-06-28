#include <openxr/openxr.h>

#include <glwindow/glwindow.hh>

#include <opengl/opengl.hh>

#include <glfw/glfw.hh>

#ifdef OS_WINDOWS
#define XR_USE_PLATFORM_WIN32 1
#define XR_USE_GRAPHICS_API_OPENGL 1
#else
#define XR_USE_PLATFORM_XLIB 1
#define XR_USE_GRAPHICS_API_OPENGL 1
#endif

#include <openxr/openxr_platform.h>

#include <core/core.hh>

using namespace sgf;

int main() {

	uint width = 1280;
	uint height = 720;

	auto window = new GLWindow("OpenXR", width, height);

	window->shouldClose.connect(window, &GLWindow::close);

	XrInstance instance{};
	{
		const char* exts[] = {"XR_KHR_opengl_enable"};

		XrInstanceCreateInfo info{XR_TYPE_INSTANCE_CREATE_INFO};
		strcpy(info.applicationInfo.applicationName, "Testapp");
		info.applicationInfo.applicationVersion = XR_VERSION_1_0;
		strcpy(info.applicationInfo.engineName, "mak3d");
		info.applicationInfo.engineVersion = XR_VERSION_1_0;
		info.applicationInfo.apiVersion = 0x0001000000000000L;
		info.enabledExtensionCount = std::size(exts);
		info.enabledExtensionNames = exts;

		verify(xrCreateInstance(&info, &instance) == 0);
		debug() << "### Successfully created instance";
	}

	XrSystemId systemId{};
	XrSystemProperties props{};
	{
		XrSystemGetInfo info{XR_TYPE_SYSTEM_GET_INFO};
		info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

		verify(xrGetSystem(instance, &info, &systemId) == 0);
		debug() << "### Successfully created systemId";

		verify(xrGetSystemProperties(instance, systemId, &props) == 0);
		debug() << "### System properties:" << props.systemName
				<< "width:" << props.graphicsProperties.maxSwapchainImageWidth
				<< "Height:" << props.graphicsProperties.maxSwapchainImageHeight << "Layers"
				<< props.graphicsProperties.maxLayerCount;
	}

	XrSession session{};
	{
		PFN_xrGetOpenGLGraphicsRequirementsKHR xrGetOpenGLGraphicsRequirementsKHR = nullptr;
		verify(xrGetInstanceProcAddr(instance, "xrGetOpenGLGraphicsRequirementsKHR",
									 (PFN_xrVoidFunction*)(&xrGetOpenGLGraphicsRequirementsKHR)) == 0);

		XrGraphicsRequirementsOpenGLKHR requirements{};
		verify(xrGetOpenGLGraphicsRequirementsKHR(instance, systemId, &requirements) == 0);
		debug() << "### Requirements max api version:" << XR_VERSION_MAJOR(requirements.maxApiVersionSupported) << "."
				<< XR_VERSION_MINOR(requirements.maxApiVersionSupported);
		debug() << "### Requirements min api version:" << XR_VERSION_MAJOR(requirements.minApiVersionSupported) << "."
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
		info.systemId = systemId;

		verify(xrCreateSession(instance, &info, &session) == 0);
		debug() << "### Successfully created session";
	}

	XrSpace viewSpace{};
	XrSpace localSpace{};
	{
		XrReferenceSpaceCreateInfo info{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
		info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
		info.poseInReferenceSpace.orientation.w = 1;
		verify(xrCreateReferenceSpace(session, &info, &viewSpace) == 0);

		info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
		info.poseInReferenceSpace.orientation.w = 1;
		info.poseInReferenceSpace.position.z = 1;
		verify(xrCreateReferenceSpace(session, &info, &localSpace) == 0);
		debug() << "### Created view and local spaces";
	}

	XrSwapchain swapchain{};
	std::vector<XrSwapchainImageOpenGLKHR> images;
	std::vector<GLuint> framebuffers;
	{
		uint32_t n;
		verify(xrEnumerateSwapchainFormats(session, 0, &n, nullptr) == 0);
		std::vector<int64_t> formats(n);
		verify(xrEnumerateSwapchainFormats(session, n, &n, formats.data()) == 0);
		debug() << "### Successfully enumed formats";

		XrSwapchainCreateInfo info{XR_TYPE_SWAPCHAIN_CREATE_INFO};
		info.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
		info.format = formats[0]; // GL_RGBA16;
		info.sampleCount = 1;
		info.width = props.graphicsProperties.maxSwapchainImageWidth;
		info.height = props.graphicsProperties.maxSwapchainImageHeight;
		info.faceCount = 1;
		info.arraySize = 1;
		info.mipCount = 1;
		verify(xrCreateSwapchain(session, &info, &swapchain) == 0);
		debug() << "### Successfully created swapchain";

		verify(xrEnumerateSwapchainImages(swapchain, 0, &n, nullptr) == 0);
		images.resize(n);
		verify(xrEnumerateSwapchainImages(swapchain, n, &n, (XrSwapchainImageBaseHeader*)images.data()) == 0);
		debug() << "### Enumed swapchain images:" << n;

		framebuffers.resize(n);
		verify(glGetError() == 0);
		glGenFramebuffers(n, framebuffers.data());
		for (uint32_t i = 0; i < n; ++i) {
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, images[i].image, 0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		verify(glGetError() == 0);
	}

	bool ready = false;

	window->run([instance, session, swapchain, framebuffers, viewSpace, &ready] {
		XrEventDataBuffer event{XR_TYPE_EVENT_DATA_BUFFER};

		if (!xrPollEvent(instance, &event)) {
			switch (event.type) {
			case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
				auto ev = reinterpret_cast<XrEventDataSessionStateChanged*>(&event);
				if (ev->state == XR_SESSION_STATE_READY) {
					debug() << "### Session state ready!";
					verify(!ready);
					verify(ev->session == session);
					XrSessionBeginInfo info{XR_TYPE_SESSION_BEGIN_INFO};
					info.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
					verify(xrBeginSession(session, &info) == 0);
					debug() << "### Successfully started session";
					ready = true;
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

		if (!ready) return;

		uint32_t image{};
		verify(xrAcquireSwapchainImage(swapchain, nullptr, &image) == 0);

		// TODO: handle timeout here....
		XrSwapchainImageWaitInfo waitinfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
		waitinfo.timeout = 100000000L; // 100 ms
		verify(xrWaitSwapchainImage(swapchain, &waitinfo) == 0);

		XrFrameState state{};
		verify(xrWaitFrame(session, nullptr, &state) == 0);

		verify(xrBeginFrame(session, nullptr) == 0);

		if (state.shouldRender) {

			// debug() << "### Rendering";
			verify(!glGetError());
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[image]);
			glViewport(0, 0, 1512, 1680);
			glClearColor(1, .5f, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			verify(!glGetError());
		}

		// Need to release before endFrame as "xrEndFrame will use the most recently released swapchain image"?
		verify(xrReleaseSwapchainImage(swapchain, nullptr) == 0);

		{
			// Get current pose/fov per eye
			XrView views[2]{};
			XrViewState vstate{};
			{
				XrViewLocateInfo vinfo = {XR_TYPE_VIEW_LOCATE_INFO};
				vinfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
				vinfo.displayTime = state.predictedDisplayTime;
				vinfo.space = viewSpace;

				uint32_t n;
				verify(xrLocateViews(session, &vinfo, &vstate, 2, &n, views) == 0);
			}

			XrCompositionLayerProjectionView projViews[2]{};
			{
				for (int i = 0; i < 2; ++i) {
					auto& projView = projViews[i];
					projView.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
					projView.pose = views[i].pose;
					projView.fov = views[i].fov;
					projView.subImage.swapchain = swapchain;
					projView.subImage.imageRect.extent.width = 1512;
					projView.subImage.imageRect.extent.height = 1680;
				}
			}

			XrCompositionLayerProjection layer0{XR_TYPE_COMPOSITION_LAYER_PROJECTION};
			layer0.space = viewSpace;
			layer0.viewCount = 2;
			layer0.views = projViews;
			XrCompositionLayerBaseHeader* layers[1] = {(XrCompositionLayerBaseHeader*)&layer0};

			XrFrameEndInfo endinfo{XR_TYPE_FRAME_END_INFO};
			endinfo.displayTime = state.predictedDisplayTime;
			endinfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
			endinfo.layerCount = 1;
			endinfo.layers = layers;

			//            debug() << "### EndFrame!";
			auto r = xrEndFrame(session, &endinfo);
			if (r) { debug() << "endFrame failed!" << r; }

			// verify(xrEndFrame(session, &endinfo) == 0);
		}
	});

	verify(xrDestroySwapchain(swapchain) == 0);
	debug() << "### Successfully destroyed swapchain";

	verify(xrDestroySession(session) == 0);
	debug() << "### Successfully destroyed session";

	verify(xrDestroyInstance(instance) == 0);
	debug() << "### Successfully destroyed instance";

	return 0;
}
