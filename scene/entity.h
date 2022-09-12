#pragma once

#include <geom/geom.hh>

namespace sgf {

class Scene;

class Entity : public Object {
public:
	SGF_OBJECT_TYPE(Entity, Object);

	Scene* const scene{};

	Entity();

	void setName(String name) {
		m_name = std::move(name);
	}

	CString name() const {
		return m_name;
	}

	void setParent(Entity* parent);

	Entity* parent() const {
		return m_parent;
	}

	CVector<Entity*> children() const {
		return m_children;
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

	CMat3f worldRotationMatrix() const {
		return m_worldMatrix.m;
	}

	Quatf worldQuaternion() const {
		return Quatf(m_worldMatrix.m);
	}

	Vec3f worldRotation() const {
		return m_worldMatrix.m.rotation();
	}

	void setMatrix(CAffineMat4f matrix) {
		m_matrix = matrix;
		m_dirty = Dirty::euler;
	}

	CAffineMat4f matrix() const {
		if (m_dirty == Dirty::matrix) {
			m_matrix.m = Mat3f::rotation(m_rotation);
			m_dirty = Dirty::none;
		}
		return m_matrix;
	}

	void setPosition(CVec3f position) {
		m_matrix.t = position;
	}

	CVec3f position() const {
		return m_matrix.t;
	}

	void setRotationMatrix(CMat3f matrix) {
		m_matrix.m = matrix;
		m_dirty = Dirty::euler;
	}

	CMat3f rotationMatrix() const {
		return matrix().m;
	}

	void setRotation(CVec3f rotation) {
		m_rotation = rotation;
		m_dirty = Dirty::matrix;
	}

	CVec3f rotation() const {
		if (m_dirty == Dirty::euler) {
			m_rotation = m_matrix.m.rotation();
			m_dirty = Dirty::none;
		}
		return m_rotation;
	}

	void setQuaternion(Quatf quat) {
		m_matrix.m = Mat3f(quat);
		m_dirty = Dirty::euler;
	}

	Quatf quaternion() const {
		return (m_dirty == Dirty::euler) ? Quatf(m_matrix.m) : Quatf::rotation(m_rotation);
	}

	void setScale(CVec3f scale) {
		m_scale = scale;
	}

	CVec3f scale() const {
		return m_scale;
	}

	void translate(CVec3f v) {
		m_matrix.t += m_matrix.m * v;
	}

	void postTranslate(CVec3f v) {
		m_matrix.t += v;
	}

	void rotate(CVec3f rotation) {
		m_matrix.m = matrix().m * Mat3f::rotation(rotation);
		m_dirty = Dirty::euler;
	}

	void lookAt(CVec3f target, CVec3f up = {0, 1, 0});

protected:
	static inline bool g_worldMatrixUpdated;

	void updateWorldMatrix(CAffineMat4f worldMatrix) {
		m_worldMatrix = worldMatrix;
		g_worldMatrixUpdated = true;
	}

	virtual void onEnable();

	virtual void onDisable();

	virtual void onUpdate();

private:
	enum struct Dirty { none = 0, matrix = 1, euler = 2 };

	String m_name;
	AffineMat4f m_worldMatrix;
	Entity* m_parent{};
	Vector<Entity*> m_children;
	bool m_enabled{};

	mutable Dirty m_dirty{Dirty::none};
	mutable AffineMat4f m_matrix;
	mutable Vec3f m_rotation;
	mutable Vec3f m_scale{1};
};

} // namespace sgf
