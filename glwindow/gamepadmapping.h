#pragma once

#include <core/core.hh>

namespace sgf {

struct GamepadMapping;

const GamepadMapping* findGamepadMapping(ushort vendorId, ushort productId);

float getGamepadInput(const GamepadMapping* mapping, uint index, bool button, const float* axes, uint numAxes, const uchar* buttons,
					  uint numButtons, const uchar* hats, uint numHats);

} // namespace sgf
