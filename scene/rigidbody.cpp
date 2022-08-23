#include "rigidbody.h"

namespace sgf{

RigidBody::RigidBody(Scene* scene) : Entity(scene) {

	m_inverseInertiaTensor = Mat3f(2.0f / 5.0f).inverse();
}

void RigidBody::addForce(CVec3f force, CVec3f point) {

}

void RigidBody::addImpulse(CVec3f impulse, CVec3f point) {

	m_linearVelocity += impulse * m_inverseMass;

	m_angularVelocity += m_inverseInertiaTensor * impulse.cross(point);
}

void RigidBody::onEnable() {
	m_position=worldPosition();
	m_orientation=worldMatrix().m;
}

void RigidBody::onDisable() {
}

void RigidBody::onUpdate() {

	float elapsed=1.0f/60.0f;

	// Update position

	Vec3f linearAccel = Vec3f(0, -9.81f, 0);

	linearAccel += m_forces * m_inverseMass;

	m_linearVelocity += linearAccel * elapsed;

	m_position += m_linearVelocity * elapsed;

	m_linearVelocity *= std::pow(m_linearDamping, elapsed);

	m_forces = {};

	// Update orientation

	Vec3f angularAccel = m_inverseInertiaTensor * m_torques;

	m_angularVelocity += angularAccel * elapsed;

	m_orientation += m_orientation * Quatf(m_angularVelocity, 0) * (elapsed * .5f);

	m_angularVelocity *= std::pow(m_angularDamping, elapsed);

	m_orientation.normalize();

	m_torques = {};

	// Update entity

	updateWorldMatrix({m_orientation, m_position});
}

}
