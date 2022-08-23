#pragma once

#include <geom/geom.hh>

namespace sgf {

class Scene;

class Entity : public Object {
public:
	SGF_OBJECT_TYPE(Entity, Object);

	Entity(Scene* scene) : m_scene(scene) {
	}

	Scene* scene() const {
		return m_scene;
	}

	void addChild(Entity* child);

	void removeChild(Entity* child);

	void setParent(Entity* parent);

	Entity* parent() const {
		return m_parent;
	}

	void setMatrix(CAffineMat4f matrix);

	CAffineMat4f matrix() const {
		return m_matrix;
	}

	void setPosition(CVec3f position) {
		m_matrix.t = position;
	}

	CVec3f position() const {
		return m_matrix.t;
	}

	void setRotation(CVec3f rotation) {
		m_rotation = rotation;
		m_matrix.m = Mat3f::rotation(m_rotation) * Mat3f::scale(m_scale);
	}

	CVec3f rotation() const {
		return m_rotation;
	};

	void setScale(CVec3f scale) {
		m_scale = scale;
		m_matrix.m = Mat3f::rotation(m_rotation) * Mat3f::scale(m_scale);
	}

	CVec3f scale() const {
		return m_scale;
	}

	void setEnabled(bool enabled) {
		enabled ? enable() : disable();
	}

	bool enabled() const {
		return m_enabled;
	}

	void enable();

	void disable();

	void update();

	CAffineMat4f worldMatrix() const {
		return m_worldMatrix;
	}

	CVec3f worldPosition() const {
		return m_worldMatrix.t;
	}

	void translate(CVec3f v) {
		m_matrix.t += m_matrix.m * v;
	}

	void rotate(CVec3f rotation) {
		m_matrix.m = Mat3f::rotation(m_rotation) * Mat3f::rotation(rotation) * Mat3f::scale(m_scale);
		m_rotation = m_matrix.m.rotation();
	}

	void lookAt(CVec3f target, CVec3f up = {0, 1, 0});

protected:
	void updateWorldMatrix(CAffineMat4f worldMatrix) {
		assert(!g_worldMatrixUpdated);
		m_worldMatrix = worldMatrix;
		g_worldMatrixUpdated=true;
	}

	virtual void onEnable() {
	}

	virtual void onDisable() {
	}

	virtual void onUpdate();

private:
	static bool g_worldMatrixUpdated;
	Scene* m_scene{};
	Entity* m_parent{};
	Vector<Entity*> m_children;
	AffineMat4f m_worldMatrix;
	AffineMat4f m_matrix;
	Vec3f m_rotation;
	Vec3f m_scale{1};
	bool m_enabled = false;
};

} // namespace sgf
