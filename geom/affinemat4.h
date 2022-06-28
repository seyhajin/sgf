#pragma once

#include "box.h"
#include "line.h"
#include "mat3.h"
#include "plane.h"
#include "vec3.h"

#include <core/core.hh>

namespace sgf {

template <class T> struct AffineMat4;
template <class T> using CAffineMat4 = const AffineMat4<T>&;
using AffineMat4f = AffineMat4<float>;
using CAffineMat4f = CAffineMat4<float>;

template <class T> struct AffineMat4 {

	using CAffineMat4f = const AffineMat4<T>&;

	Mat3<T> m;
	Vec3<T> t;

	AffineMat4() = default;

	AffineMat4(CVec3<T> t) : t(t) {
	}
	AffineMat4(CMat3<T> m) : m(m) {
	}
	AffineMat4(CMat3<T> m, CVec3<T> t) : m(m), t(t) {
	}
	AffineMat4(T ix, T iy, T iz, T jx, T jy, T jz, T kx, T ky, T kz, T tx, T ty, T tz)
		: m(ix, iy, iz, jx, jy, jz, kx, ky, kz), t(tx, ty, tz) {
	}

	bool operator==(CAffineMat4f that) const {
		return m == that.m && t == that.t;
	}

	bool operator!=(CAffineMat4f that) const {
		return !operator==(that);
	}

	Vec3<T> operator*(CVec3<T> v) const {
		return {m.i.x * v.x + m.j.x * v.y + m.k.x * v.z + t.x, m.i.y * v.x + m.j.y * v.y + m.k.y * v.z + t.y,
				m.i.z * v.x + m.j.z * v.y + m.k.z * v.z + t.z};
	}

	Box<T> operator*(CBox<T> b) const {
		Box<T> r;
		for (int i = 0; i < 8; ++i) r |= operator*(b.corner(i));
		return r;
	}

	Line<T> operator*(CLine<T> l) const {
		return {operator*(l.o), m * l.d};
	}

	Plane<T> operator*(CPlane<T> p) const {
		return {operator*(p.n * -p.d), m * p.n};
	}

	AffineMat4 operator*(CAffineMat4<T> r) const {
		return {m * r.m, m * r.t + t};
	}

	AffineMat4& operator*=(CAffineMat4<T> r) const {
		m *= r.m;
		t += m * r.t;
		return *this;
	}

	AffineMat4 inverse() const {
		auto i = m.inverse();
		return {i, i * -t};
	}

	AffineMat4 transpose() const {
		auto i = m.transpose();
		return {i, i * -t};
	}

	CMat3f rotation() const {
		return m;
	}

	Mat3f& rotation() {
		return m;
	}

	CVec3f position() const {
		return t;
	}

	Vec3f& position() {
		return t;
	}

	static AffineMat4 pitch(float v) {
		return Mat3<T>::pitch(v);
	}
	static AffineMat4 yaw(float v) {
		return Mat3<T>::yaw(v);
	}
	static AffineMat4 roll(float v) {
		return Mat3<T>::roll(v);
	}
	static AffineMat4 translation(CVec3<T> v) {
		return AffineMat4<T>(v);
	}
	static AffineMat4 position(CVec3<T> v) {
		return AffineMat4<T>(v);
	}
	static AffineMat4 rotation(CVec3<T> v) {
		return Mat3<T>::rotation(v);
	}
	static AffineMat4 scale(CVec3<T> v) {
		return Mat3<T>::scale(v);
	}

	friend std::ostream& operator<<(std::ostream& str, CAffineMat4<T> m) {
		return str << '(' << m.m.i << ',' << m.m.j << ',' << m.m.k << ',' << m.t << ')';
	}
};

} // namespace sgf
