#include "rigidbody.h"

#include "collider.h"
#include "intersect.h"
#include "scene.h"

#include <imgui/imgui.hh>

namespace sgf {

void RigidBody::warpToWorldPosition(CVec3f position) {
	assert(enabled());

	updateWorldMatrix({worldRotationMatrix(), position});
	m_position = position;
}

void RigidBody::addForce(CVec3f force, CVec3f point) {
	assert(enabled());
}

void RigidBody::addImpulse(CVec3f impulse, CVec3f point) {
	assert(enabled());

	m_linearVelocity += impulse * m_inverseMass;
	m_angularVelocity += m_worldInverseInertiaTensor * impulse.cross(point);
}

void RigidBody::addCollisionForces(RigidBody* body2, CVec3f cpoint, CVec3f cnormal, CPhysicsMaterial physmat) {
	assert(enabled());
	assert(isUnit(cnormal.length()));

	Vec3f pt1 = cpoint - m_position;
	Vec3f pt2 = body2 ? cpoint - body2->m_position : Vec3f();

	Vec3f rvel = m_linearVelocity + pt1.cross(m_angularVelocity);
	if (body2) rvel -= body2->m_linearVelocity + pt2.cross(body2->m_angularVelocity);

	if (rvel.dot(cnormal) >= 0) return;

	{
		float d = m_inverseMass;
		Vec3f v = (m_worldInverseInertiaTensor * pt1.cross(cnormal)).cross(pt1);
		if (body2) {
			d += body2->m_inverseMass;
			v += (body2->m_worldInverseInertiaTensor * pt2.cross(cnormal)).cross(pt2);
		}
		d += cnormal.dot(v);

		if (d > 0) {
			float n = rvel.dot(cnormal) * -(physmat.bounciness + 1);
			float j = n / d;
			Vec3f impulse = j * cnormal;

			addImpulse(impulse, pt1);
			if (body2) body2->addImpulse(-impulse, pt2);
		}
	}

	if (physmat.friction <= 0) return;

	{
		Vec3f ctangent = -(rvel - cnormal * rvel.dot(cnormal));
		float len = ctangent.length();
		if (len <= unitLengthEpsilon) return;

		ctangent /= len;

		float fd = m_inverseMass;
		Vec3f fv = (m_worldInverseInertiaTensor * pt1.cross(ctangent)).cross(pt1);
		if (body2) {
			fd += body2->m_inverseMass;
			fv += (body2->m_worldInverseInertiaTensor * pt2.cross(ctangent)).cross(pt2);
		}
		fd += ctangent.dot(fv);

		if (fd > 0) {
			float fn = -rvel.dot(ctangent);
			float fj = fn / fd;
			Vec3f impulse = fj * physmat.friction * ctangent;

			addImpulse(impulse, pt1);
			if (body2) body2->addImpulse(-impulse, pt2);
		}
	}
}

void RigidBody::onEnable() {
	m_position = worldPosition();
	m_rotation = Quatf(worldRotationMatrix());
	m_worldInverseInertiaTensor = worldRotationMatrix() * m_inverseInertiaTensor * worldRotationMatrix().transpose();
	scene->addEntity(this);
}

void RigidBody::onDisable() {
	scene->removeEntity(this);
}

void RigidBody::onUpdate() {

	float elapsed = 1.0f / 60.0f;

	// Update position

	Vec3f linearAccel = Vec3f(0, -9.81f, 0);

	linearAccel += m_forces * m_inverseMass;

	m_linearVelocity += linearAccel * elapsed;

	m_position += m_linearVelocity * elapsed;

	m_linearVelocity *= std::pow(m_linearDamping, elapsed);

	m_forces = {};

	// Update orientation

	Vec3f angularAccel = m_worldInverseInertiaTensor * m_torques;
	m_angularVelocity += angularAccel * elapsed;

	m_rotation += Quatf(m_angularVelocity * elapsed * .5f, 0) * m_rotation;
	m_rotation.normalize();

	m_angularVelocity *= std::pow(m_angularDamping, elapsed);

	m_torques = {};

	// Update entity

	updateWorldMatrix({Mat3f(m_rotation), m_position});

	m_worldInverseInertiaTensor = worldRotationMatrix() * m_inverseInertiaTensor * worldRotationMatrix().transpose();
}

} // namespace sgf
