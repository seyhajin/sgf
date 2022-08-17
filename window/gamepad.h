#pragma once

#include "gamepadmapping.h"
#include "uidevice.h"

namespace sgf {

struct SDLMapping;

class Gamepad : public UIDevice {
public:
	static constexpr uint numAxes = 4;
	static constexpr uint numButtons = 17;

	String name() const {
		return m_name;
	};

	String mapping() const {
		return m_mapping ? "standard" : "";
	};

	bool connected() const {
		return m_connected;
	}

	float axis(uint index) const {
		assert(index < numAxes);
		return m_axes[index];
	}

private:
	friend class GLWindow;

	uint m_id;
	bool m_connected = false;
	String m_name;
	const GamepadMapping* m_mapping = nullptr;
	float m_axes[numAxes]{};

	Gamepad(uint id);

	void poll(const float* axes, int numAxes, const uchar* buttons, int numButtons, const uchar* hats, int numHats);

	void onPoll() override;
};

} // namespace sgf
