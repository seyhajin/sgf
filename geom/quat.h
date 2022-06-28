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

	T i = 0;
	T j = 0;
	T k = 0;
	T r = 1;

	Quat() = default;

	Quat(CVec3<T> v, float r) : i(v.x), j(v.y), k(v.z), r(r) {
	}

	Quat(T i, T j, T k, T r) : i(i), j(j), k(k), r(r) {
	}

	Quat(CMat3<T> m) {
		static constexpr float epsilon = 0;

		T t = m.i.x + m.j.y + m.k.z;

		if (t > epsilon) {
			t = std::sqrt(t + 1) * 2;
			i = (m.k.y - m.j.z) / t;
			j = (m.i.z - m.k.x) / t;
			k = (m.j.x - m.i.y) / t;
			r = t * .25f;
		} else if (m.i.x > m.j.y && m.i.x > m.k.z) {
			t = std::sqrt(m.i.x - m.j.y - m.k.z + 1) * 2;
			i = t * .25f;
			j = (m.j.x + m.i.y) / t;
			k = (m.i.z + m.k.x) / t;
			r = (m.k.y - m.j.z) / t;
		} else if (m.j.y > m.k.z) {
			t = std::sqrt(m.j.y - m.k.z - m.i.x + 1) * 2;
			i = (m.j.x + m.i.y) / t;
			j = t * .25f;
			k = (m.k.y + m.j.z) / t;
			r = (m.i.z - m.k.x) / t;
		} else {
			t = std::sqrt(m.k.z - m.j.y - m.i.x + 1) * 2;
			i = (m.i.z + m.k.x) / t;
			j = (m.k.y + m.j.z) / t;
			k = t * .25f;
			r = (m.j.x - m.i.y) / t;
		}
	}

	Quat operator~() const {
		return {-i, -j, -k, r};
	}

	Vec3f operator*(CVec3<T> v) const {
		Quat q = operator*(Quat(v, 0)) * operator~();
		return {q.i, q.j, q.k};
	}

	Quat operator*(T s) const {
		return {i * s, j * s, k * s, r * s};
	}

	Quat operator*(CQuat q) const {
		// clang-format off
		return {
			r * q.i + i * q.r + j * q.k - k * q.j,
			r * q.j + j * q.r + k * q.i - i * q.k,
			r * q.k + k * q.r + i * q.j - j * q.i,
			r * q.r - i * q.i - j * q.j - k * q.k
		};
		// clang-format on
	}

	Quat operator+(CQuat q) const {
		return {i + q.i, j + q.j, k + q.k, r + q.r};
	}

	Quat& operator*=(T s) {
		return *this = operator*(s);
	}

	Quat& operator*=(CQuat q) {
		return *this = operator*(q);
	}

	Quat& operator+=(CQuat q) {
		return *this = operator+(q);
	}

	Mat3<T> toMat3() const {
		return {*this};
	}

	T length() const {
		return std::sqrt(i * i + j * j + k * k + r * r);
	}

	Quat normalized() const {
		T d = 1 / length();
		return {i * d, j * d, k * d, r * d};
	}

	void normalize() {
		T d = 1 / length();
		i *= d;
		j *= d;
		k *= d;
		r *= d;
	}

	friend std::ostream& operator<<(std::ostream& str, CQuat q) {
		return str << '(' << q.i << ',' << q.j << ',' << q.k << ',' << q.r << ')';
	}
};

template <class T> Mat3<T>::Mat3(const Quat<T>& q) {
	float ii = q.i * q.i, jj = q.j * q.j, kk = q.k * q.k;
	float ij = q.i * q.j, ik = q.i * q.k, jk = q.j * q.k;
	float ir = q.i * q.r, jr = q.j * q.r, kr = q.k * q.r;

	i = {1 - 2 * (jj + kk), 2 * (ij - kr), 2 * (ik + jr)};
	j = {2 * (ij + kr), 1 - 2 * (ii + kk), 2 * (jk - ir)};
	k = {2 * (ik - jr), 2 * (jk + ir), 1 - 2 * (ii + jj)};

	assert(isUnit(i.length()));
	assert(isUnit(j.length()));
	assert(isUnit(k.length()));
}

template <class T> Quat<T> Mat3<T>::toQuat() const {
	return *this;
}

} // namespace sgf
