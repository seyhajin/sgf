#pragma once

#include "imguiex.h"

#include <geom/geom.hh>
#include <imgui/imgui.hh>

struct GLFWwindow;

namespace sgf {

class Keyboard;
class Gamepad;

namespace settings {
extern Property<bool> showDebugLog;
extern Property<bool> showDebugInfo;
extern Property<bool> fullScreen;
extern Property<bool> vsyncEnabled;
} // namespace settings

namespace stats {
extern Property<float> refreshRate;
extern Property<uint> FPS;
extern Property<uint> cpuIdle;
extern Property<uint> gpuIdle;
} // namespace stats

class GLWindow : public Object {
public:
	SGF_OBJECT_TYPE(GLWindow, Object);

	static constexpr uint maxGamepads = 4;

	Property<bool> keyEventsEnabled{true};

	Signal<uint, bool> gamepadConnectedChanged;

	Signal<Vec2i> sizeChanged;
	Signal<> shouldClose;
	Signal<> gotFocus;
	Signal<> lostFocus;
	Signal<> updating;

	GLWindow(CString title, uint width, uint height);
	~GLWindow();

	CVec2i position() const {
		return m_position;
	}

	CVec2i size() const {
		return m_size;
	}

	bool closed() const {
		return m_glfwWindow == nullptr;
	}

	bool focus() const {
		return m_focus;
	}

	Keyboard* keyboard() const {
		return m_keyboard;
	}

	Gamepad* gamepad(uint index) const {
		assert(index < maxGamepads);
		return m_gamepads[index];
	}

	GLFWwindow* glfwWindow() const {
		return m_glfwWindow;
	};

	void run(Function<void()> runFunc);

	void close();

private:
	static GLWindow* getWindow(GLFWwindow* glfwWindow);

	GLFWwindow* m_glfwWindow;

	Keyboard* m_keyboard;
	Gamepad* m_gamepads[maxGamepads];

	bool m_focus = true;
	Vec2i m_position;
	Vec2i m_size;

	Vec2i m_restorePosition;
	Vec2i m_restoreSize;

	double m_idleTime;
	double m_fpsTime;
	uint m_fps;

	Function<void()> m_runFunc;

	ImGuiEx::ImGuiLog m_log;

	void drawDebugLog();
	void drawDebugInfo();
	void updateFPS();
	void updateIdleStats();
	void updateEvents();
	void swapBuffers();
	void singleStep();
	void clearHitKeys();
};

} // namespace sgf
