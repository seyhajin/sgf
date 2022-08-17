#pragma once

#include "uidevice.h"

struct GLFWwindow;

namespace sgf {

class Keyboard : public UIDevice {
public:
	static constexpr uint maxButtons = 512;

	Keyboard(GLFWwindow* window);

	Button& key(uint index) const {
		return button(index);
	}
};

} // namespace sgf
