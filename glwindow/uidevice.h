#pragma once

#include <geom/geom.hh>

namespace sgf {

class GLWindow;
class Keyboard;
class Gamepad;

class Button : public Object {
public:
	Signal<float> valueChanged;
	Signal<> pressed;

	float value() const {
		return m_value;
	}

	bool down() const {
		return m_down;
	}

	bool hit() const {
		return m_hit;
	}

private:
	friend class UIDevice;

	float m_value = 0;
	bool m_down = false;
	bool m_hit = false;
};

class UIDevice : public Object {
public:
	static constexpr float buttonDownThreshold = .1f;

	uint maxButtons() const {
		return m_maxButtons;
	}

	Button& button(uint index) const {
		assert(index < m_maxButtons);
		return m_buttons[index];
	}

	void flush();

	void resetButtonHits();

	void update();

protected:
	UIDevice(uint maxButtons);
	virtual ~UIDevice() = default;

	void setButtonValue(uint index, float value);

	void setButtonDown(uint index, bool down);

	virtual void onPoll() {
	}

private:
	friend class GLWindow;

	uint m_maxButtons;
	Button* m_buttons;
	PodVector<uint> m_hits;

	void setButtonState(uint index, float value, bool down);
};

} // namespace sgf
