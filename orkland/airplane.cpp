#include "airplane.h"

#include <window/window.hh>

namespace sgf {

void Airplane::reset() {
	m_pitchVel=0;
	m_pitch=0;
	m_yawVel=0;
	m_yaw=0;
	m_roll=0;
	m_speed = minSpeed;
}

void Airplane::onUpdate() {

	auto window = mainWindow();

	// pitch
	if (window->keyboard()->key(SGF_KEY_UP).down()) {
		m_pitchVel += (maxPitchVel - m_pitchVel) * pitchAccel;
	} else if (window->keyboard()->key(SGF_KEY_DOWN).down()) {
		m_pitchVel += (-maxPitchVel - m_pitchVel) * pitchAccel;
	}else {
		m_pitchVel -= m_pitchVel * pitchDamping;
	}
	m_pitch += m_pitchVel;

	// yaw
	if (window->keyboard()->key(SGF_KEY_LEFT).down()) {
		m_yawVel += (maxYawVel - m_yawVel) * yawAccel;
	} else if (window->keyboard()->key(SGF_KEY_RIGHT).down()) {
		m_yawVel += (-maxYawVel - m_yawVel) * yawAccel;
	} else {
		m_yawVel -= m_yawVel * yawDamping;
	}
	m_yaw += m_yawVel;

	// roll
	m_roll = m_yawVel * rollScale;

	setRotation({m_pitch, m_yaw, m_roll});

	// throttle
	if (window->keyboard()->key(SGF_KEY_A).down()) {
		m_speed = std::min(m_speed + speedAccel, maxSpeed);
	} else if (window->keyboard()->key(SGF_KEY_Z).down()) {
		m_speed = std::max(m_speed - speedAccel, minSpeed);
	}
	translate({0, 0, m_speed});
}

} // namespace sgf
