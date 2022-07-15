#include "glwindow.h"

#include "gamepad.h"
#include "keyboard.h"

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

namespace settings {
Property<bool> showDebugInfo(true);
Property<bool> showDebugLog(true);
Property<bool> vsyncEnabled(true);
Property<bool> fullScreen;
} // namespace settings

namespace stats {
Property<float> refreshRate;
Property<uint> FPS;
Property<uint> cpuIdle;
Property<uint> gpuIdle;
} // namespace stats

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

		GLFWmonitor* monitor = nullptr;

#ifndef __EMSCRIPTEN__
		// TODO: Get this working...
		glfwWindowHint(GLFW_SRGB_CAPABLE, 0);
		if (settings::fullScreen) monitor = glfwGetPrimaryMonitor();
		stats::refreshRate = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
#else
		get_initial_canvas_size(&width, &height);
		stats::refreshRate = 60;
#endif

		m_glfwWindow = glfwCreateWindow(width, height, title.c_str(), monitor, nullptr);
		verify(m_glfwWindow);

		glfwSetWindowUserPointer(m_glfwWindow, this);

		glfwMakeContextCurrent(m_glfwWindow);

		initGLContext();

		settings::vsyncEnabled.valueChanged.connect([](bool enabled) { glfwSwapInterval(enabled ? 1 : 0); });

		glfwSwapInterval(settings::vsyncEnabled ? 1 : 0);
	}

	// Init window resize handling
	{
		glfwSetFramebufferSizeCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int width, int height) {
			auto window = getWindow(glfwWindow);
			window->m_size = {width, height};
			window->sizeChanged.emit(window->m_size);
		});
		glfwGetWindowPos(m_glfwWindow, &m_position.x, &m_position.y);
		glfwGetFramebufferSize(m_glfwWindow, &m_size.x, &m_size.y);
	}

	// Init fullscreen switching handling
	{
#ifndef __EMSCRIPTEN__
		settings::fullScreen.valueChanged.connect(this, [this](bool fullScreenMode) {
			if (fullScreenMode) {
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
		if (settings::fullScreen) {
			m_restorePosition = Vec2i(16, 16);
			m_restoreSize -= Vec2i(32, 32);
		} else {
			m_restorePosition = m_position;
			m_restoreSize = m_size;
		}
#endif
	}

	// Init window focus handling
	{
#ifdef __EMSCRIPTEN__
		auto focuscb = [](int type, const EmscriptenFocusEvent* event, void* user) -> int {
			auto window = static_cast<GLWindow*>(user);
			window->m_focus = (type == EMSCRIPTEN_EVENT_FOCUS);
			(void)(window->m_focus ? window->gotFocus.emit() : window->lostFocus.emit());
			return 1;
		};
		emscripten_set_focus_callback("canvas", this, true, focuscb);
		emscripten_set_blur_callback("canvas", this, true, focuscb);
#else
		glfwSetWindowFocusCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int focused) {
			auto window = getWindow(glfwWindow);
			window->m_focus = focused;
			(void)(focused ? window->gotFocus.emit() : window->lostFocus.emit());
		});
#endif
	}

	// Init UI devices
	{
		m_keyboard = new Keyboard();
		glfwSetKeyCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
			auto window = getWindow(glfwWindow);
			if (window->keyEventsEnabled) window->m_keyboard->sendKeyEvent(key, scancode, action, mods);
		});

		for (uint id = 0; id < maxGamepads; ++id) m_gamepads[id] = new Gamepad(id);
		glfwSetJoystickCallback([](int id, int event) {});
	}

	// Add some shortcut keys
	{
		auto keyToggle = [this](uint key, Property<bool>* value) {
			m_keyboard->key(key).pressed.connect([value] { *value = !*value; });
		};
		keyToggle(GLFW_KEY_I, &settings::showDebugInfo);
		keyToggle(GLFW_KEY_L, &settings::showDebugLog);
		keyToggle(GLFW_KEY_V, &settings::vsyncEnabled);
		keyToggle(GLFW_KEY_F11, &settings::fullScreen);
	}

	// Init ImGui
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
		ImGui_ImplOpenGL3_Init(nullptr);
	}

	// Init debug console
	{
		auto updateDebugLogFunc = [this](bool show) {
			return;
			if (show) {
				debugOutputFunc = [this](CString str) { m_log.AddLog("%s\n", str.c_str()); };
			} else {
				debugOutputFunc = defaultDebugOutputFunc;
			}
		};
		settings::showDebugLog.valueChanged.connect([updateDebugLogFunc](bool show) { updateDebugLogFunc(show); });
		updateDebugLogFunc(settings::showDebugLog);
	}

	// Init Timing stats vars
	{
		m_idleTime = glfwGetTime();
		m_fpsTime = m_idleTime;
		m_fps = 0;
	}
}

GLWindow::~GLWindow() {
	if (this == g_mainWindow) g_mainWindow = nullptr;

	close();
}

void GLWindow::close() {

	if (!m_glfwWindow) return;

	if (this == g_running) g_running = nullptr;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_glfwWindow);
	m_glfwWindow = nullptr;

	glfwTerminate();
}

void GLWindow::drawDebugLog() {

	assert(settings::showDebugLog);

	ImGui::StyleColorsClassic();

	bool show = true;
	ImGui::Begin("Debug log", &show);
	if (!show) {
		settings::showDebugLog = false;
		ImGui::End();
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 6));

	ImGui::SetNextWindowPos(ImVec2(m_size.x / 4, 0), ImGuiCond_Once);			  // ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(m_size.x / 2, m_size.y / 2), ImGuiCond_Once); // ImGuiCond_FirstUseEver);
	m_log.Draw("Debug log", &show);

	ImGui::PopStyleVar(3);

	ImGui::End();
}

void GLWindow::drawDebugInfo() {

	assert(settings::showDebugInfo);

	//	ImGui::StyleColorsDark();

	//	ImGui::ShowDemoWindow();

	ImGui::StyleColorsClassic();

	bool show = true;
	ImGui::Begin("Debug", &show);
	if (!show) {
		settings::showDebugInfo = false;
		ImGui::End();
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 6));

	ImGui::Text("FPS: %i  CPU Idle: %i%%  GPU Idle: %i%%", uint(stats::FPS), uint(stats::cpuIdle),
				uint(stats::gpuIdle));

	ImGuiEx::Checkbox("Full screen mode", settings::fullScreen);
	ImGuiEx::Checkbox("VSync enabled", settings::vsyncEnabled);

	if (ImGui::CollapsingHeader("OpenGL info")) {

		ImGui::Text("VENDOR: %s", glGetString(GL_VENDOR));
		ImGui::Text("RENDERER: %s", glGetString(GL_RENDERER));
		ImGui::Text("VERSION: %s", glGetString(GL_VERSION));
		ImGui::Text("SHADING_LANGUAGE_VERSION: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

		GLint num;
		glGetIntegerv(GL_NUM_EXTENSIONS, &num);
		int lineHeight = ImGui::GetTextLineHeightWithSpacing();
		if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 10 * lineHeight))) {
			for (int i = 0; i < num; ++i) { ImGui::Text("%s", (const char*)glGetStringi(GL_EXTENSIONS, i)); }
			ImGui::EndListBox();
		}
	}

	ImGui::PopStyleVar(3);

	ImGui::End();
}

void GLWindow::updateFPS() {
	++m_fps;
	double now = glfwGetTime();
	if (now - m_fpsTime >= 1) {
		stats::FPS = m_fps;
		m_fpsTime = now;
		m_fps = 0;
	}
}

void GLWindow::updateIdleStats() {
	assert(!settings::vsyncEnabled);

	double frameDur = 1.0 / stats::refreshRate;

	double cpuElapsed = glfwGetTime() - m_idleTime;
	stats::cpuIdle = (frameDur - cpuElapsed) / frameDur * 100;

	glFinish();

	double gpuElapsed = glfwGetTime() - m_idleTime;
	stats::gpuIdle = (frameDur - gpuElapsed) / frameDur * 100;

	double elapsed = gpuElapsed;
	m_idleTime += elapsed;

	double delay = frameDur - elapsed;

	if (delay > 0 && delay < frameDur) {
		glfwWaitEventsTimeout(delay);
		m_idleTime += delay;
	}
}

void GLWindow::updateEvents() {

	glfwPollEvents();
	if (glfwWindowShouldClose(m_glfwWindow)) {
		if (shouldClose.connected()) {
			shouldClose.emit();
		} else {
			close();
		}
		return;
	}

	// Begin ImGui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Update UI Devices
	m_keyboard->beginUpdate();
#ifdef __EMSCRIPTEN__
	emscripten_sample_gamepad_data();
#endif
	for (uint i = 0; i < maxGamepads; ++i) {
		auto gp = m_gamepads[i];
		bool con = gp->connected();
		gp->beginUpdate();
		if (gp->connected() != con) gamepadConnectedChanged.emit(i, !con);
	}
}

void GLWindow::swapBuffers() {

	m_keyboard->endUpdate();
	for (auto gp : m_gamepads) gp->endUpdate();

	if (settings::showDebugLog) drawDebugLog();
	if (settings::showDebugInfo) drawDebugInfo();

	// Render ImGui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (!settings::vsyncEnabled) updateIdleStats();

	glfwSwapBuffers(m_glfwWindow);

	updateFPS();
}

void GLWindow::singleStep() {

	SharedPtrPool sharedPtrPool;

	updateEvents();
	if (!m_glfwWindow) return;

	pollAppEvents();

	updating.emit();

	m_runFunc();

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

} // namespace sgf
