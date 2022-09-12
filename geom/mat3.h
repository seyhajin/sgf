#pragma once

#include <core/core.hh>

#include "vec3.h"

namespace sgf {

template <class T> struct Quat;
template <class T> struct Mat3;
template <class T> using CMat3 = const Mat3<T>&;

using Mat3f = Mat3<float>;
using CMat3f = CMat3<float>;

template <class T> struct Mat3 {

	using CMat3 = const Mat3<T>&;

	Vec3<T> i{1, 0, 0};
	Vec3<T> j{0, 1, 0};
	Vec3<T> k{0, 0, 1};

	Mat3() = default;

	Mat3(T s) : i(s, 0, 0), j(0, s, 0), k(0, 0, s) {
	}

	Mat3(CVec3<T> i, CVec3<T> j, CVec3<T> k) : i(i), j(j), k(k) {
	}

	Mat3(T ix, T iy, T iz, T jx, T jy, T jz, T kx, T ky, T kz) : i{ix, iy, iz}, j{jx, jy, jz}, k{kx, ky, kz} {
	}

	explicit Mat3(CVec3<T> s) : i(s.x, 0, 0), j(0, s.y, 0), k(0, 0, s.z) {
	}

	// implemented in quat.h
	explicit Mat3(const Quat<T>& quat);

	bool operator==(CMat3 that) const {
		return i == that.i && j == that.j && k == that.k;
	}

	bool operator!=(CMat3 that) const {
		return !operator==(that);
	}

	Vec3<T> operator*(CVec3<T> v) const {
		return {i.x * v.x + j.x * v.y + k.x * v.z, i.y * v.x + j.y * v.y + k.y * v.z,
				i.z * v.x + j.z * v.y + k.z * v.z};
	}

	Mat3<T> operator*(CMat3 m) const {
		return {i.x * m.i.x + j.x * m.i.y + k.x * m.i.z, i.y * m.i.x + j.y * m.i.y + k.y * m.i.z,
				i.z * m.i.x + j.z * m.i.y + k.z * m.i.z, i.x * m.j.x + j.x * m.j.y + k.x * m.j.z,
				i.y * m.j.x + j.y * m.j.y + k.y * m.j.z, i.z * m.j.x + j.z * m.j.y + k.z * m.j.z,
				i.x * m.k.x + j.x * m.k.y + k.x * m.k.z, i.y * m.k.x + j.y * m.k.y + k.y * m.k.z,
				i.z * m.k.x + j.z * m.k.y + k.z * m.k.z};
	}

	Mat3& operator*=(CMat3 m) {
		return *this = operator*(m);
	}

	T determinant() const {
		return i.x * (j.y * k.z - j.z * k.y) - i.y * (j.x * k.z - j.z * k.x) + i.z * (j.x * k.y - j.y * k.x);
	}

	Mat3 inverse() const {
		T t = 1 / determinant();
		return {t * (j.y * k.z - j.z * k.y),  -t * (i.y * k.z - i.z * k.y), t * (i.y * j.z - i.z * j.y),
				-t * (j.x * k.z - j.z * k.x), t * (i.x * k.z - i.z * k.x),	-t * (i.x * j.z - i.z * j.x),
				t * (j.x * k.y - j.y * k.x),  -t * (i.x * k.y - i.y * k.x), t * (i.x * j.y - i.y * j.x)};
	}

	Mat3 transpose() const {
		return {i.x, j.x, k.x, i.y, j.y, k.y, i.z, j.z, k.z};
	}

	Mat3 cofactor() const {
		return {(j.y * k.z - j.z * k.y),  -(j.x * k.z - j.z * k.x), (j.x * k.y - j.y * k.x),
				-(i.y * k.z - i.z * k.y), (i.x * k.z - i.z * k.x),	-(i.x * k.y - i.y * k.x),
				(i.y * j.z - i.z * j.y),  -(i.x * j.z - i.z * j.x), (i.x * j.y - i.y * j.x)};
	}

	T yaw() const {
		return k.yaw();
	}

	T pitch() const {
		return k.pitch();
	}

	T roll() const {
		return std::atan2(i.y, j.y);
	}

	Vec3<T> rotation() const {
		return {pitch(), yaw(), roll()};
	}

	Vec3<T> scale() const {
		return {i.length(), j.length(), k.length()};
	}

	static Mat3 yaw(T r) {
		float s = std::sin(r), c = std::cos(r);
		return {c, 0, s, 0, 1, 0, -s, 0, c};
	}

	static Mat3 pitch(T r) {
		float s = std::sin(r), c = std::cos(r);
		return {1, 0, 0, 0, c, s, 0, -s, c};
	}

	static Mat3 roll(T r) {
		float s = std::sin(r), c = std::cos(r);
		return {c, s, 0, -s, c, -0, 0, 0, 1};
	}

	static Mat3 rotation(CVec3<T> r) {
		// OPTIMIZE ME!
		return yaw(r.y) * pitch(r.x) * roll(r.z);
	}

	static Mat3 scale(CVec3<T> s) {
		return {s.x, 0, 0, 0, s.y, 0, 0, 0, s.z};
	}

	friend std::ostream& operator<<(std::ostream& str, CMat3 m) {
		return str << '(' << m.i << ',' << m.j << ',' << m.k << ')';
	}
};

} // namespace sgf
