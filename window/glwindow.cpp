#include "glwindow.h"

#include "gamepad.h"
#include "keyboard.h"
#include "mouse.h"

#include <opengl/opengl.hh>
// Shouldn't have to force opengl b4 glfw? glfw still including gl.h for some reason?
#include <glfw/glfw.hh>

#ifdef OS_EMSCRIPTEN

#include "emscripten.h"
#include "emscripten/html5.h"

EM_JS(void, get_initial_canvas_size, (uint32_t * width, uint32_t* height), {
	var canvas = document.getElementById('canvas');
	var rect = canvas.getBoundingClientRect();
	HEAP32[width >> 2] = rect.width;
	HEAP32[height >> 2] = rect.height;
});

#endif

namespace sgf {

namespace {

GLWindow* g_mainWindow;
GLWindow* g_running;
Function<void()> g_runFunc;

} // namespace

GLWindow* GLWindow::getWindow(GLFWwindow* glfwWindow) {
	return static_cast<GLWindow*>(glfwGetWindowUserPointer(glfwWindow));
}

GLWindow::GLWindow(CString title, uint width, uint height) {
	assert(!g_mainWindow);
	g_mainWindow = this;

	// Init GLFW and GLFWwindow
	{
		glfwSetErrorCallback([](int err, const char* desc) {
			fprintf(stderr, "GLFW error %i: %s\n", err, desc);
			panic("OOPS");
		});

		if (!glfwInit()) panic("GLFWinit() failed");

#ifdef USE_OPENGLES
#ifdef WIN32
		// Angle is EGL so we need this
		glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
#endif
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 0);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

#ifdef OS_EMSCRIPTEN
		get_initial_canvas_size(&width, &height);
#endif

		// FIXME: gives very different results than pow(rgba,1/2.2) or whatever...
		// glfwWindowHint(GLFW_SRGB_CAPABLE, 0);

		m_glfwWindow = glfwCreateWindow(int(width), int(height), title.c_str(), nullptr, nullptr);
		if (!m_glfwWindow) panic("Failed to create GLFW window!");

		glfwSetWindowUserPointer(m_glfwWindow, this);

		glfwMakeContextCurrent(m_glfwWindow);

		// Init GLEW on desktop
		initGLContext();
	}

	// Window resize handling
	{
		glfwSetFramebufferSizeCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int width, int height) {
			auto window = getWindow(glfwWindow);
			window->m_size = {width, height};
			window->sizeChanged.emit(window->m_size);
		});
		glfwGetWindowPos(m_glfwWindow, &m_position.x, &m_position.y);
		glfwGetFramebufferSize(m_glfwWindow, &m_size.x, &m_size.y);
	}

	// Fullscreen mode handling
	{
#ifndef OS_EMSCRIPTEN
		fullScreenMode.valueChanged.connect(this, [this](bool fullScreen) {
			if (fullScreen) {
				glfwGetWindowPos(m_glfwWindow, &m_restorePosition.x, &m_restorePosition.y);
				glfwGetWindowSize(m_glfwWindow, &m_restoreSize.x, &m_restoreSize.y);
				auto monitor = glfwGetPrimaryMonitor();
				auto mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(m_glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			} else {
				glfwSetWindowMonitor(m_glfwWindow, nullptr, m_restorePosition.x, m_restorePosition.y, m_restoreSize.x,
									 m_restoreSize.y, 0);
			}
		});
		if (fullScreenMode) {
			m_restorePosition = Vec2i(16, 16);
			m_restoreSize -= Vec2i(32, 32);
		} else {
			m_restorePosition = m_position;
			m_restoreSize = m_size;
		}
#endif
	}

	// Vsync control handling
	{
#ifndef OS_EMSCRIPTEN
		vsyncEnabled.valueChanged.connect([](bool enabled) { glfwSwapInterval(enabled ? 1 : 0); });
		glfwSwapInterval(vsyncEnabled ? 1 : 0);
#endif
	}

	// Focus handling
	{
#ifdef __EMSCRIPTEN__
		auto focuscb = [](int type, const EmscriptenFocusEvent* event, void* user) -> int {
			auto window = static_cast<GLWindow*>(user);
			window->m_hasFocus = (type == EMSCRIPTEN_EVENT_FOCUS);
			(void)(window->m_hasFocus ? window->gotFocus.emit() : window->lostFocus.emit());
			return 1;
		};
		emscripten_set_focus_callback("canvas", this, true, focuscb);
		emscripten_set_blur_callback("canvas", this, true, focuscb);
#else
		glfwSetWindowFocusCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int focused) {
			auto window = getWindow(glfwWindow);
			window->m_hasFocus = focused;
			(void)(focused ? window->gotFocus.emit() : window->lostFocus.emit());
		});
#endif
	}

	// Init UI devices
	{
		m_keyboard = new Keyboard(m_glfwWindow);

		m_mouse = new Mouse(m_glfwWindow);

		for (uint id = 0; id < maxGamepads; ++id) m_gamepads.push_back(new Gamepad(id));

		glfwSetJoystickCallback([](int id, int event) {});
	}

	// Init Timing stats vars
	{
		m_idleTime = glfwGetTime();
		m_fpsTime = m_idleTime;
		m_fps = 0;
	}
}

GLWindow::~GLWindow() {

	close(); // NOLINT
}

void GLWindow::close() {

	if (!m_glfwWindow) return;

	if (this == g_mainWindow) g_mainWindow = nullptr;

	if (this == g_running) g_running = nullptr;

	glfwDestroyWindow(m_glfwWindow);
	m_glfwWindow = nullptr;

	glfwTerminate();
}

void GLWindow::updateFPS() {
	++m_fpsTicks;
	double now = glfwGetTime();
	if (now - m_fpsTime >= 1) {
		m_fps = m_fpsTicks;
		m_fpsTime = now;
		m_fpsTicks = 0;
	}
}

void GLWindow::updateIdleStats() {
	assert(!vsyncEnabled);

	double frameDur = 1.0 / 60.0; // m_refreshRate;

	double cpuElapsed = glfwGetTime() - m_idleTime;
	m_cpuIdle = uint((frameDur - cpuElapsed) / frameDur * 100);

	glFinish();

	double gpuElapsed = glfwGetTime() - m_idleTime;
	m_gpuIdle = uint((frameDur - gpuElapsed) / frameDur * 100);

	double elapsed = gpuElapsed;
	m_idleTime += elapsed;

	double delay = frameDur - elapsed;

	if (delay > 0 && delay < frameDur) {
		glfwWaitEventsTimeout(delay);
		m_idleTime += delay;
	}
}

void GLWindow::beginFrame() {

	updateFPS();

	m_keyboard->resetButtonHits();
	m_mouse->resetButtonHits();
	for (auto gp : m_gamepads) gp->resetButtonHits();

	glfwPollEvents();

	pollAppEvents();

	if (m_glfwWindow && glfwWindowShouldClose(m_glfwWindow)) {
		if (shouldClose.connected()) {
			shouldClose.emit();
		} else {
			close();
		}
		return;
	}

	// Update UI Devices
	m_keyboard->update();
	m_mouse->update();
#ifdef __EMSCRIPTEN__
	emscripten_sample_gamepad_data();
#endif
	for (uint i = 0; i < maxGamepads; ++i) {
		auto gp = m_gamepads[i];
		bool connected = gp->connected();
		gp->update();
		if (gp->connected() != connected) gamepadConnectedChanged.emit(i, !connected);
	}
}

void GLWindow::endFrame() {

	//	if (!settings::vsyncEnabled) updateIdleStats();
}

void GLWindow::swapBuffers() {

	if (m_glfwWindow) glfwSwapBuffers(m_glfwWindow);
}

void GLWindow::singleStep() {

	SharedPtrPool sharedPtrPool;

	beginFrame();

	if (m_glfwWindow) m_runFunc();

	endFrame();

	swapBuffers();
}

void GLWindow::run(Function<void()> runFunc) {

	assert(!g_running);
	g_running = this;
	m_runFunc = runFunc;

#ifdef __EMSCRIPTEN__
	auto stepper = [] {
		if (g_running) g_running->singleStep();
	};
	emscripten_set_main_loop(stepper, 0, true);
#else
	while (g_running) singleStep();
#endif
}

void GLWindow::stop() {

	assert(g_running);
	g_running = nullptr;
	m_runFunc = {};
};

} // namespace sgf
