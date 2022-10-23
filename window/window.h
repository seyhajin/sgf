#pragma once

#include "keycodes.h"

#include <geom/geom.hh>
#include <glfw/glfw.hh>

namespace sgf {

class Keyboard;
class Mouse;
class Gamepad;

class Window : public Object {
public:
	SGF_OBJECT_TYPE(Window, Object);

	using FrameFunc = Function<void()>;

	Property<bool> vsyncEnabled{true};
	Property<bool> fullScreen{false};
	Property<bool> inputEventsEnabled{true};

	Signal<uint, bool> gamepadConnectedChanged;
	Signal<CVec2i> sizeChanged;
	Signal<> shouldClose;
	Signal<> gotFocus;
	Signal<> lostFocus;

	virtual CVec2i size() const = 0;

	virtual Keyboard* keyboard() const =0;
	virtual Mouse* mouse() const = 0;
	virtual CVector<Gamepad*> gamepads() const = 0;

	virtual void close() = 0;

	virtual void run(FrameFunc frameFunc) = 0;
	virtual void stop() = 0;

	virtual void beginFrame() = 0;
	virtual void endFrame() = 0;

protected:
	Window() = default;

	~Window() override = default;

private:
	static inline Window* g_mainWindow;

	friend Window* createMainWindow(CString title,uint width, uint height);

	friend Window* mainWindow();
};

Window* createMainWindow(CString title,uint width, uint height);

inline Window* mainWindow() {
	return Window::g_mainWindow;
}

} // namespace sgf
