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
	m_hits.push_back(index);
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
	assert(index < m_maxButtons);

	auto& b = m_buttons[index];
	if (b.m_down == down) return;

	setButtonState(index, float(down), down);
}

void UIDevice::flush() {
	for (auto b = m_buttons; b < m_buttons + m_maxButtons; ++b) { b->m_down = b->m_hit = false; }
	m_hits.clear();
}

void UIDevice::resetButtonHits() {
	for (auto index : m_hits) m_buttons[index].m_hit = false;
	m_hits.clear();
}

void UIDevice::update() {
	onPoll();
}

} // namespace sgf
