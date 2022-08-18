#pragma once

#include "keycodes.h"

#include <geom/geom.hh>

#include <GLFW/glfw3.h>

namespace sgf {

class Keyboard;
class Mouse;
class Gamepad;

class Window : public Object {
public:
	SGF_OBJECT_TYPE(Window, Object);

	Property<bool> vsyncEnabled{true};
	Property<bool> fullScreenMode{false};
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

	virtual void run(Function<void()> frameFunc) = 0;
	virtual void stop() = 0;

	virtual void beginFrame() = 0;
	virtual void endFrame() = 0;
};

Window* createWindow(CString title,uint width, uint height);

} // namespace sgf
