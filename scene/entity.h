#pragma once

#include <geom/geom.hh>

namespace sgf {

class Scene;

class Entity : public Object {
public:
	SGF_OBJECT_TYPE(Entity, Object);

	explicit Entity(Entity* parent);

	void setParent(Entity* parent);

	Entity* parent() const {
		return m_parent;
	}

	CVector<Entity*> children() const {
		return m_children;
	}

	AffineMat4f& matrix() {
		return m_matrix;
	}

	CMat3f rotation() const {
		return m_matrix.rotation();
	}

	Mat3f& rotation() {
		return m_matrix.rotation();
	}

	CVec3f& position() const {
		return m_matrix.position();
	}

	Vec3f& position() {
		return m_matrix.position();
	}

	void updateWorldMatrix();

	CAffineMat4f worldMatrix() const {
		return m_worldMatrix;
	}

	void setEnabled(bool enabled);

	bool enabled() const {
		return m_enabled;
	}

	Scene* scene() const;

protected:
	virtual void onSetEnabled(bool enabled) {
	}

	virtual void onUpdateWorldMatrix() {
	}

private:
	Entity* m_parent;
	Vector<Entity*> m_children;
	AffineMat4f m_worldMatrix;
	AffineMat4f m_matrix;
	bool m_enabled = false;
};

} // namespace sgf
