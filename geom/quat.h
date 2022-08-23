#pragma once

#include <core/core.hh>

#include "mat3.h"
#include "vec3.h"

namespace sgf {

template <class T> struct Quat;
template <class T> using CQuat = const Quat<T>&;
using Quatf = Quat<float>;
using CQuatf = CQuat<float>;

template <class T> struct Quat {

	using CQuat = const Quat<T>&;
	using CVec3 = const Vec3<T>&;

	Vec3<T> v;
	T w = 1;

	Quat() = default;

	Quat(CVec3 v, T w) : v(v), w(w) {
	}

	Quat(CMat3<T> m) {
		static constexpr float epsilon = 0;

		T t = m.i.x + m.j.y + m.k.z;

		if (t > epsilon) {
			t = std::sqrt(t + 1) * 2;
			v = Vec3<T>((m.k.y - m.j.z) / t, (m.i.z - m.k.x) / t, (m.j.x - m.i.y) / t);
			w = t * .25f;
		} else if (m.i.x > m.j.y && m.i.x > m.k.z) {
			t = std::sqrt(m.i.x - m.j.y - m.k.z + 1) * 2;
			v = Vec3<T>(t * .25f, (m.j.x + m.i.y) / t, (m.i.z + m.k.x) / t);
			w = (m.k.y - m.j.z) / t;
		} else if (m.j.y > m.k.z) {
			t = std::sqrt(m.j.y - m.k.z - m.i.x + 1) * 2;
			v = Vec3<T>((m.j.x + m.i.y) / t, t * .25f, (m.k.y + m.j.z) / t);
			w = (m.i.z - m.k.x) / t;
		} else {
			t = std::sqrt(m.k.z - m.j.y - m.i.x + 1) * 2;
			v = Vec3<T>((m.i.z + m.k.x) / t, (m.k.y + m.j.z) / t, t * .25f);
			w = (m.j.x - m.i.y) / t;
		}
	}

	Quat operator-() const {
		return {-v, w};
	}

	Quat operator+(CQuat q) const {
		return {v + q.v, w + q.w};
	}

	Quat operator*(CQuat q) const {
		return {q.v.cross(v) + q.v * w + v * q.w, w * q.w - v.dot(q.v)};
	}

	Quat operator*(T s) const {
		return {v * s, w * s};
	}

	Quat& operator+=(CQuat q) {
		return *this = *this + q;
	}

	Quat& operator*=(CQuat q) {
		return *this = *this * q;
	}

	Quat& operator*=(T s) {
		return *this = *this * s;
	}

	Vec3f operator*(CVec3 qv) const {
		return (*this * Quatf(qv, 0) * -*this).v;
	}

	T dot(CQuat& q) const {
		return v.dot(q.v) + w * q.w;
	}

	T length() const {
		return std::sqrt(dot(*this));
	}

	Quat normalized() const {
		T sc = 1 / length();
		return {v * sc, w * sc};
	}

	void normalize() {
		T sc = 1 / length();
		v *= sc;
		w *= sc;
	}

	Vec3<T> i() const {
		T ix = 1 - (v.y * v.y + v.z * v.z) * 2;
		T iy = (v.x * v.y - v.z * w) * 2;
		T iz = (v.x * v.z + v.y * w) * 2;
		return {ix, iy, iz};
	}

	Vec3<T> j() const {
		T jx = (v.x * v.y + v.z * w) * 2;
		T jy = 1 - (v.x * v.x + v.z * v.z) * 2;
		T jz = (v.y * v.z - v.x * w) * 2;
		return {jx, jy, jz};
	}

	Vec3<T> k() const {
		T kx = (v.x * v.z - v.y * w) * 2;
		T ky = (v.y * v.z + v.x * w) * 2;
		T kz = 1 - (v.x * v.x + v.y * v.y) * 2;
		return {kx, ky, kz};
	}

	T yaw() const {
		T kx = (v.x * v.z - v.y * w) * 2;
		T kz = 1 - (v.x * v.x + v.y * v.y) * 2;
		return -std::atan2(kx, kz);
	}

	T pitch() const {
		T kx = (v.x * v.z - v.y * w) * 2;
		T ky = (v.y * v.z + v.x * w) * 2;
		T kz = 1 - (v.x * v.x + v.y * v.y) * 2;
		return -std::atan2(ky, sqrtf(kx * kx + kz * kz));
	}

	T roll() const {
		T iy = (v.x * v.y - v.z * w) * 2;
		T jy = 1 - (v.x * v.x + v.z * v.z) * 2;
		return std::atan2(iy, jy);
	}

	Vec3<T> rotation() const {
		return {pitch(), yaw(), roll()};
	}

	static Quat pitch(T r) {
		return {Vec3<T>(std::sin(r / -2), 0, 0), std::cos(r / -2)};
	}

	static Quat yaw(T r) {
		return {Vec3<T>(0, std::sin(r / 2), 0), std::cos(r / 2)};
	}

	static Quat roll(T r) {
		return {Vec3<T>(0, 0, std::sin(r / -2)), std::cos(r / -2)};
	}

	static Quat rotation(CVec3 r) {
		return yaw(r.y) * pitch(r.x) * roll(r.z);
	}

	friend std::ostream& operator<<(std::ostream& str, CQuat q) {
		return str << '(' << q.v << ',' << q.w << ')';
	}
};

template <class T> Mat3<T>::Mat3(const Quat<T>& q) {
	float xx = q.v.x * q.v.x, yy = q.v.y * q.v.y, zz = q.v.z * q.v.z;
	float xy = q.v.x * q.v.y, xz = q.v.x * q.v.z, yz = q.v.y * q.v.z;
	float wx = q.w * q.v.x, wy = q.w * q.v.y, wz = q.w * q.v.z;

	i = Vec3<T>(1 - 2 * (yy + zz), 2 * (xy - wz), 2 * (xz + wy));
	j = Vec3<T>(2 * (xy + wz), 1 - 2 * (xx + zz), 2 * (yz - wx));
	k = Vec3<T>(2 * (xz - wy), 2 * (yz + wx), 1 - 2 * (xx + yy));
}

} // namespace sgf
