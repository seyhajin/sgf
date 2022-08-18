#pragma once

#include "window.h"

struct GLFWwindow;

namespace sgf {

class GLWindow : public Window {
public:
	SGF_OBJECT_TYPE(GLWindow, Window);

	static constexpr uint maxGamepads=4;

	GLWindow(CString title, uint width, uint height);

	~GLWindow() override;

	CVec2i size() const override{
		return m_size;
	}

	Keyboard* keyboard() const override{
		return m_keyboard;
	}

	Mouse* mouse() const override{
		return m_mouse;
	}

	CVector<Gamepad*> gamepads() const override{
		return m_gamepads;
	}

	void close() override;

	void beginFrame() override;

	void endFrame() override;

	void run(Function<void()> frameFunc) override;

	void stop() override;

	GLFWwindow* glfwWindow() const {
		return m_glfwWindow;
	};

	static GLWindow* getWindow(GLFWwindow* glfwWindow);

private:
	GLFWwindow* m_glfwWindow{};

	Keyboard* m_keyboard{};
	Mouse* m_mouse{};
	Vector<Gamepad*> m_gamepads{};

	Vec2i m_position;
	Vec2i m_size;
	bool m_hasFocus{};

	Vec2i m_restorePosition;
	Vec2i m_restoreSize;

	double m_idleTime;
	double m_fpsTime;
	uint m_cpuIdle{};
	uint m_gpuIdle{};
	uint m_fpsTicks{};
	uint m_fps{};

	Function<void()> m_runFunc;

	void updateFPS();
	void updateIdleStats();
	void swapBuffers();
	void singleStep();
};

} // namespace sgf
