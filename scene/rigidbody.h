#pragma once

#include "entity.h"

namespace sgf {

class RigidBody : public Entity {
public:
	RigidBody(Scene* scene);

	void setMass(float mass) {
		m_inverseMass = 1.0f / mass;
	}

	float inverseMass() const {
		return m_inverseMass;
	}

	void setInertiaTensor(CMat3f inertiaTensor) {
		m_inverseInertiaTensor = inertiaTensor.inverse();
	}

	CMat3f inverseInertiaTensor() const {
		return m_inverseInertiaTensor;
	}
	
	void setLinearDamping(float linearDamping) {
		m_linearDamping = linearDamping;
	}
	
	float linearDamping()const {
		return m_linearDamping;
	}
	
	void setAngularDamping(float angularDamping) {
		m_angularDamping = angularDamping;
	}
	
	float angularDamping()const {
		return m_angularDamping;
	}
	
	void setLinearVelocity(CVec3f linearVelocity) {
		m_linearVelocity = linearVelocity;
	}
	
	CVec3f linearVelocity()const {
		return m_linearVelocity;
	}
	
	void setAngularVelocity(CVec3f angularVelocity) {
		m_angularVelocity = angularVelocity;
	}
	
	CVec3f angularVelocity()const {
		return m_angularVelocity;
	}
	
	void clearForces() {
		m_forces = m_torques = {};
	}

	void addForce(CVec3f force, CVec3f point);

	void addImpulse(CVec3f impulse, CVec3f point);

private:
	float m_inverseMass{1};
	Vec3f m_forces;
	float m_linearDamping{.8f};
	Vec3f m_linearVelocity;
	Vec3f m_position;

	Mat3f m_inverseInertiaTensor;
	Vec3f m_torques;
	float m_angularDamping{.8f};
	Vec3f m_angularVelocity;
	Quatf m_orientation;

	void onEnable() override;

	void onDisable() override;

	void onUpdate()override;
};

} // namespace sgf
