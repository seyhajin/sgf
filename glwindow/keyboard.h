#pragma once

#include "uidevice.h"

namespace sgf {

class Keyboard : public UIDevice {
public:
	Button& key(uint index) const {
		return button(index);
	}

private:
	friend class GLWindow;

	Keyboard();

	void sendKeyEvent(int key, int scancode, int action, int mods);
};

} // namespace sgf
