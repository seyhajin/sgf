#include "entity.h"

#include "scene.h"

namespace sgf {
bool Entity::g_worldMatrixUpdated;

void Entity::addChild(Entity* child) {
	assert(!child->m_parent);
	m_children.push_back(child);
	child->m_parent = this;
	child->setEnabled(enabled());
}

void Entity::removeChild(Entity* child) {
	assert(child->m_parent == this);
	bool wasEnabled = child->enabled();
	child->disable();
	remove(m_children, child);
	child->m_parent = nullptr;
	child->setEnabled(wasEnabled);
}

void Entity::setMatrix(CAffineMat4f matrix) {
	m_matrix = matrix;
	m_rotation = matrix.m.rotation(); // Cache me?
	m_scale = matrix.m.scale();		  // Cache me?
}

void Entity::enable() {
	if (m_enabled) return;
	m_enabled = true;
	onEnable();
	for (auto e : m_children) e->enable();
}

void Entity::disable() {
	if (!m_enabled) return;
	m_enabled = false;
	onDisable();
	for (auto e : m_children) e->disable();
}

void Entity::update() {
	g_worldMatrixUpdated = false;
	onUpdate();
	if (!g_worldMatrixUpdated) Entity::onUpdate();
	for (auto e : m_children) e->update();
}

void Entity::onUpdate() {
	updateWorldMatrix(m_parent ? m_parent->worldMatrix() * matrix() : matrix());
}

void Entity::lookAt(CVec3f target, CVec3f up) {
	Vec3f k = target - m_matrix.t;
	Vec3f i = up.cross(k);
	Vec3f j = k.cross(i).normalized();
	m_matrix.m = Mat3f(i.normalized(), j.normalized(), k.normalized());
	m_rotation = m_matrix.m.rotation();
}

} // namespace sgf
