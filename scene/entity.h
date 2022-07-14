#pragma once

#include <geom/geom.hh>

namespace sgf {

class Entity : public Object {
public:
	SGF_OBJECT_TYPE(Entity, Object);

	void setMatrix(CAffineMat4f matrix) {
		m_matrix = matrix;
		m_rotation = matrix.m.rotation();
		m_scale = matrix.m.scale();
		m_dirty = Dirty::rotationMatrix;
	}

	CAffineMat4f matrix() const {
		if(bool(m_dirty & Dirty::rotationMatrix)){
			assert(!bool(m_dirty & Dirty::rotation));
			m_matrix.m = Mat3f::rotation(m_rotation);
			m_dirty = Dirty::none;
		}
		return m_matrix;
	};

	void setPosition(CVec3f position) {
		m_matrix.t = position;
	}

	CVec3f position() const {
		return m_matrix.t;
	}

	void setRotation(CVec3f rotation) {
		m_rotation = rotation;
		m_dirty = Dirty::rotationMatrix;
	}

	CVec3f rotation() const {
		if (bool(m_dirty & Dirty::rotation)) {
			assert(!bool(m_dirty & Dirty::rotationMatrix));
			m_rotation = m_matrix.m.rotation();
			m_dirty = Dirty::none;
		}
		return m_rotation;
	}

	void setRotatonMatrix(CMat3f rotationMatrix) {
		m_matrix.m=rotationMatrix;
		m_dirty = Dirty::rotation;
	}

	CMat3f rotationMatrix() const {
		if (bool(m_dirty & Dirty::rotationMatrix)) {
			assert(!bool(m_dirty & Dirty::rotation));
			m_matrix.m = Mat3f::rotation(m_rotation);
			m_dirty &= ~Dirty::rotationMatrix;
		}
		return m_matrix.m;
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

	void rotate(CVec3f r) {
		m_matrix.m=rotationMatrix() * Mat3f::rotation(r);
		m_dirty = Dirty::rotation;
	}

	void lookAt(CVec3f p, CVec3f up = {0, 1, 0}) {
		Vec3f k = p - m_matrix.t;
		Vec3f i = up.cross(k);
		Vec3f j = k.cross(i).normalized();
		m_matrix.m = Mat3f(i.normalized(), j.normalized(), k.normalized());
		m_dirty = Dirty::rotation;
	}

private:
	enum struct Dirty { none = 0, rotation = 1, rotationMatrix = 2 };

	mutable Dirty m_dirty = Dirty::none;

	mutable AffineMat4f m_matrix;
	mutable Vec3f m_rotation;
	Vec3f m_scale{1};
};

} // namespace sgf
