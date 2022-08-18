#include "gamepad.h"

#include <glfw/glfw.hh>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

namespace sgf {

// ***** Gamepad *****

Gamepad::Gamepad(uint id) : UIDevice(numButtons), m_id(id) {
}

void Gamepad::poll(const float* axes, int numDevAxes, const uchar* buttons, int numDevButtons, const uchar* hats,
				   int numDevHats) {

	for (uint i = 0; i < Gamepad::numAxes; ++i) {
		m_axes[i] = getGamepadInput(m_mapping, i, false, axes, numDevAxes, buttons, numDevButtons, hats, numDevHats);
	}

	for (uint i = 0; i < Gamepad::numButtons; ++i) {
		float value = getGamepadInput(m_mapping, i, true, axes, numDevAxes, buttons, numDevButtons, hats, numDevHats);
		setButtonValue(i, value);
	}
}

void Gamepad::onPoll() {

	bool wasConnected = m_connected;

	auto disconnected = [this, wasConnected] {
		m_mapping = nullptr;
		m_name = "";
		memset(m_axes, 0, sizeof(m_axes));
		flush();
	};

#ifdef __EMSCRIPTEN__

	EmscriptenGamepadEvent state;
	emscripten_get_gamepad_status(m_id, &state);

	m_connected = state.connected;

	if (m_connected != wasConnected) {
		if (wasConnected) {
			disconnected();
			return;
		}

		m_name = state.id;
		m_mapping = nullptr;

		if (!state.mapping[0]) {

			const char* guid = state.id;

			char buf[5]{};
			memcpy(buf, guid, 4);
			std::swap(buf[0], buf[2]);
			std::swap(buf[1], buf[3]);
			ushort vendorId = strtoul(buf, nullptr, 16);

			memcpy(buf, guid + 5, 4);
			std::swap(buf[0], buf[2]);
			std::swap(buf[1], buf[3]);
			ushort productId = strtoul(buf, nullptr, 16);

			m_mapping = findGamepadMapping(vendorId, productId);
		}
	}
	if (!m_connected) return;

	if (m_mapping) {

		float axes[16];
		for (uint i = 0; i < std::min(state.numAxes, 16); ++i) axes[i] = state.axis[i];

		uchar buttons[64];
		for (uint i = 0; i < std::min(state.numButtons, 64); ++i) buttons[i] = state.digitalButton[i];

		poll(axes, state.numAxes, buttons, state.numButtons, nullptr, 0);

	} else {

		for (uint i = 0; i < numAxes; ++i) { m_axes[i] = state.axis[i]; }
		for (uint i = 0; i < numButtons; ++i) { setButtonValue(i, state.analogButton[i]); }
	}

#else

	m_connected = glfwJoystickPresent(m_id);

	if (m_connected != wasConnected) {
		if (wasConnected) {
			disconnected();
			return;
		}

		m_name = glfwGetGamepadName(m_id);

		const char* guid = glfwGetJoystickGUID(m_id);
		assert(strlen(guid) == 32);
		char buf[5]{};
		memcpy(buf, guid + 8, 4);
		uint vendorId = strtoul(buf, nullptr, 16);
		memcpy(buf, guid + 16, 4);
		uint productId = strtoul(buf, nullptr, 16);
		m_mapping = findGamepadMapping(vendorId, productId);
	}
	if (!m_connected) return;

	int numAxes = 0;
	auto axes = glfwGetJoystickAxes(m_id, &numAxes);

	int numButtons = 0;
	auto buttons = glfwGetJoystickButtons(m_id, &numButtons);

	int numHats = 0;
	auto hats = glfwGetJoystickHats(m_id, &numHats);

	poll(axes, numAxes, buttons, numButtons, hats, numHats);

#endif
}

} // namespace sgf
