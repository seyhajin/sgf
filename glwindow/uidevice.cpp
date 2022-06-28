#include "uidevice.h"

#include <glfw/glfw.hh>

#ifdef __EMSCRIPTEN__
#include "emscripten/html5.h"
#endif

namespace sgf {

// ***** UIDevice *****

UIDevice::UIDevice(uint maxButtons) : m_maxButtons(maxButtons), m_buttons(new Button[maxButtons]) {
}

void UIDevice::setButtonState(uint index, float value, bool down) {
	assert(index >= 0 && index < m_maxButtons);

	auto& b = m_buttons[index];

	b.m_value = value;
	b.m_down = down;
	if (!down) return;

	b.m_hit = true;
	m_pressed.push_back(index);
	b.pressed.emit();
}

void UIDevice::setButtonValue(uint index, float value) {
	assert(index >= 0 && index < m_maxButtons);

	auto& b = m_buttons[index];
	if (b.m_value == value) return;

	setButtonState(index, value, value > buttonDownThreshold);

	b.valueChanged.emit(value);
}

void UIDevice::setButtonDown(uint index, bool down) {
	assert(index >= 0 && index < m_maxButtons);

	auto& b = m_buttons[index];
	if (b.m_down == down) return;

	setButtonState(index, float(down), down);
}

void UIDevice::beginUpdate() {

	onPoll();
}

void UIDevice::endUpdate() {

	for (auto index : m_pressed) { //
		m_buttons[index].m_hit = false;
	}
	m_pressed.clear();
}

void UIDevice::flush() {

	for (auto b = m_buttons; b < m_buttons + m_maxButtons; ++b) { b->m_down = b->m_hit = false; }
	m_pressed.clear();
}

} // namespace sgf
