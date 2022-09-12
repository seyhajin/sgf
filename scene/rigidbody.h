#pragma once

#include "entity.h"
#include "physicsmaterial.h"

namespace sgf {

class RigidBody : public Entity {
public:
	SGF_OBJECT_TYPE(RigidBody, Entity);

	RigidBody() = default;

	void setInverseMass(float inverseMass) {
		m_inverseMass = inverseMass;
	}

	float inverseMass() const {
		return m_inverseMass;
	}

	void setInverseInertiaTensor(CMat3f inverseInertiaTensor) {
		m_inverseInertiaTensor = inverseInertiaTensor;
		if(enabled()) m_worldInverseInertiaTensor = worldRotationMatrix() * m_inverseInertiaTensor * worldRotationMatrix().transpose();
	}

	CMat3f inverseInertiaTensor() const {
		return m_inverseInertiaTensor;
	}

	void setLinearDamping(float linearDamping) {
		m_linearDamping = linearDamping;
	}

	float linearDamping() const {
		return m_linearDamping;
	}

	void setAngularDamping(float angularDamping) {
		m_angularDamping = angularDamping;
	}

	float angularDamping() const {
		return m_angularDamping;
	}

	void setLinearVelocity(CVec3f linearVelocity) {
		m_linearVelocity = linearVelocity;
	}

	CVec3f linearVelocity() const {
		return m_linearVelocity;
	}

	void setAngularVelocity(CVec3f angularVelocity) {
		m_angularVelocity = angularVelocity;
	}

	CVec3f angularVelocity() const {
		return m_angularVelocity;
	}

	void addForce(CVec3f force, CVec3f point);

	void addImpulse(CVec3f impulse, CVec3f point);

	void warpToWorldPosition(CVec3f position);

	void addCollisionForces(RigidBody* body2, CVec3f contactPoint, CVec3f contactNormal, CPhysicsMaterial physmat);

private:
	float m_inverseMass{1};
	Vec3f m_forces;
	float m_linearDamping{1};
	Vec3f m_linearVelocity;
	Vec3f m_position;

	Mat3f m_inverseInertiaTensor{1};
	Mat3f m_worldInverseInertiaTensor{1};
	Vec3f m_torques;
	float m_angularDamping{1};
	Vec3f m_angularVelocity;
	Quatf m_rotation;

	void onEnable() override;

	void onDisable() override;

	void onUpdate() override;
};

} // namespace sgf
