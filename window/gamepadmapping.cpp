#include "gamepadmapping.h"

namespace sgf {

struct GamepadMapping {

	uint32_t deviceId;
	uchar mappings[21];
};

namespace {

using SDLMapping = GamepadMapping;

constexpr uint maxDevAxes = 16;
constexpr uint maxDevButtons = 64;
constexpr uint maxDevHats = 8;

constexpr SDLMapping defaultMapping = {0x0, {0x0,  0x1,	 0x2,  0x3,	 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
											 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50}};

// Declares: constexpr SDLMapping kSDLMappings[...] = {...};
//
#include "gamepadmappings.inc"

} // namespace

const GamepadMapping* findGamepadMapping(ushort vendorId, ushort productId) {

	uint deviceId = (vendorId << 16) | productId;

	uint low = 0;
	uint high = std::size(kSDLMappings);

	while (low != high) {
		uint mid = (low + high) / 2;
		auto mapping = &kSDLMappings[mid];
		if (deviceId == mapping->deviceId) return &kSDLMappings[mid];
		if (deviceId > mapping->deviceId) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}
	return &defaultMapping;
}

float getGamepadInput(const GamepadMapping* gpmapping, uint index, bool isButton, const float* axes, uint numAxes, const uchar* buttons,
					  uint numButtons, const uchar* hats, uint numHats) {

	uint mapping = gpmapping->mappings[index + (isButton ? 4 : 0)];

	switch (mapping & 0xc0) {
	case 0x00: {	// Axis mapping

		uint axis = mapping & (maxDevAxes - 1);
		if (axis >= numAxes) return 0;

		float value = axes[axis];
		switch ((mapping >> 4) & 3) {
		case 1:
			value = value * 2 - 1;
			break;
		case 2:
			value = value * 2 + 1;
			break;
		case 3:
			value = -value;
			break;
		}

		if (isButton) return value * .5f + .5f;
		return value;

	} break;
	case 0x40: {	// Button mapping

		uint button = mapping & (maxDevButtons - 1);
		if (button >= numButtons) return 0;

		uint value = buttons[button];

		if (isButton) return value;
		return value ? 1 : -1;

	} break;
	case 0x80: {	// Hat mapping

		if(hats) {

			uint hat = (mapping & (maxDevHats - 1)) / 2;
			if (hat >= numHats) return 0;

			uint dir = ((mapping >> 5) & 1);
			uint bit;
			if (mapping & 1) {	   // y axis?
				bit = dir ? 4 : 1; // down or up
			} else {
				bit = dir ? 2 : 8; // right or left
			}
			bool down = hats[hat / 2] & bit;

			if (isButton) return down;
			return down ? 1 : -1;

		}else {

			assert(numAxes>=2);

			uint axis = (mapping & (maxDevHats - 1)) + numAxes - 2;
			if (axis >= numAxes) return 0;

			uint dir = (mapping >> 5) & 1;

			float value = std::max(dir ? axes[axis] : -axes[axis], 0.0f);

			if(isButton) return value;
			return value * 2.0f - 1.0f;
		}
	} break;
	}
	return 0;
}

} // namespace sgf
