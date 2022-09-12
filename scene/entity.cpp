#include "entity.h"

#include "scene.h"

namespace sgf {

Entity::Entity() : scene(defaultScene()) {
}

void Entity::setParent(Entity* parent) {
	assert(!m_enabled);

	if (parent == m_parent) return;

	if (m_parent) remove(m_parent->m_children, this);

	m_parent = parent;

	if (m_parent) m_parent->m_children.push_back(this);
}

void Entity::enable() {

	assert(!m_enabled && (!m_parent || m_parent->m_enabled));

	m_enabled = true;
	Entity::onUpdate();
	onEnable();

	for (auto child : m_children) child->enable();
}

void Entity::disable() {
	assert(m_enabled);

	for (auto child : m_children) child->disable();

	m_enabled = false;
	onDisable();
}

void Entity::update() {
	if (!m_enabled) return;

	g_worldMatrixUpdated = false;
	onUpdate();
	if(!g_worldMatrixUpdated) Entity::onUpdate();

	for (auto child : m_children) child->update();
}

void Entity::onEnable() {
	scene->addEntity(this);
}

void Entity::onDisable() {
	scene->removeEntity(this);
}

void Entity::onUpdate() {
	updateWorldMatrix(m_parent ? m_parent->worldMatrix() * matrix() : matrix());
}

void Entity::lookAt(CVec3f target, CVec3f up) {
	Vec3f k = target - m_matrix.t;
	Vec3f i = up.cross(k);
	Vec3f j = k.cross(i).normalized();
	m_matrix.m = Mat3f(i.normalized(), j.normalized(), k.normalized());
}

} // namespace sgf
