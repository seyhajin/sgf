#pragma once

#include <scene/scene.hh>

namespace sgf {

class Airplane : public Actor {
public:
	static constexpr float maxSpeed = 70.0f / 60.0f;
	static constexpr float minSpeed = 20.0f / 60.0f;
	static constexpr float speedAccel = .005f;
	
	static constexpr float maxPitchVel = .01f;
	static constexpr float pitchAccel = .05f;
	static constexpr float pitchDamping = .025f;

	static constexpr float maxYawVel = .015f;
	static constexpr float yawAccel = .01f;
	static constexpr float yawDamping = .02f;

	static constexpr float rollScale = 9.0f;
	
	SGF_ACTOR_TYPE(Aircraft, Actor, 0);
	
	void reset();

private:
	float m_pitchVel{0};
	float m_pitch{0};
	float m_yawVel{0};
	float m_yaw{0};
	float m_roll{0};
	float m_speed{minSpeed};

	void onUpdate() override;
};

}
