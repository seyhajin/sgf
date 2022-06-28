#pragma once

#include <core/core.h>

namespace wb {

constexpr uint kNumGamepadAxes = 4;
constexpr uint kNumGamepadButtons = 17;

class Gamepad {
public:
	GamepadRemapper(int joy_id);

	void update();

	bool buttonDown(uint button);

private:
	float m_axes[kNumGamepadAxes];
	bool m_buttons[kNumGamepadButtons];
};

} // namespace wb
