#include "entity.h"

#include "scene.h"

namespace sgf {

Entity::Entity(Entity* parent) : m_parent(parent) {

	if (!m_parent) return;

	m_parent->m_children.push_back(this);

	setEnabled(m_parent->m_enabled);
}

void Entity::setParent(Entity* parent) {

	if (m_parent == parent) return;

	setEnabled(false);

	if (m_parent) remove(m_parent->m_children, this);

	m_parent = parent;
	if (!m_parent) return;

	m_parent->m_children.push_back(this);

	setEnabled(m_parent->m_enabled);
};

void Entity::updateWorldMatrix() {

	m_worldMatrix = m_parent ? m_parent->worldMatrix() * m_matrix : m_matrix;

	onUpdateWorldMatrix();

	for (auto child : m_children) child->updateWorldMatrix();
}

void Entity::setEnabled(bool enabled) {

	bool changed = enabled != m_enabled;
	m_enabled = enabled;

	// Enable before children...
	if (changed && enabled) onSetEnabled(true);

	for (auto child : m_children) child->setEnabled(enabled);

	// Disable after children...
	if (changed && !enabled) onSetEnabled(false);
}

Scene* Entity::scene() const {
	return activeScene();
}

} // namespace sgf
