#pragma once

#include <core/core.hh>

#include "affinemat4.h"
#include "mat3.h"
#include "vec4.h"

namespace sgf {

template <class T> struct Mat4;
template <class T> using CMat4 = const Mat4<T>&;
using Mat4f = Mat4<float>;
using CMat4f = CMat4<float>;

template <class T> struct Mat4 {

	using CMat4 = const Mat4<T>&;

	Vec4<T> i = {1, 0, 0, 0};
	Vec4<T> j = {0, 1, 0, 0};
	Vec4<T> k = {0, 0, 1, 0};
	Vec4<T> t = {0, 0, 0, 1};

	Mat4() = default;

	Mat4(CVec4<T> i, CVec4<T> j, CVec4<T> k, CVec4<T> t) : i(i), j(j), k(k), t(t) {
	}

	Mat4(T ix, T iy, T iz, T iw, T jx, T jy, T jz, T jw, T kx, T ky, T kz, T kw, T tx, T ty, T tz, T tw)
		: i(ix, iy, iz, iw), j(jx, jy, jz, jw), k(kx, ky, kz, kw), t(tx, ty, tz, tw) {
	}

	Mat4(CAffineMat4<T> m) : i(m.m.i, 0), j(m.m.j, 0), k(m.m.k, 0), t(m.t, 1) {
	}

	Mat4(CMat3<T> m) : i(m.i, 0), j(m.j, 0), k(m.k, 0) {
	}

	bool operator==(CMat4 that) const {
		return i == that.i && j == that.j && k == that.k && t == that.t;
	}

	bool operator!=(CMat4 that) const {
		return !operator==(that);
	}

	Vec4<T> operator*(CVec4<T> v) const {
		return {i.x * v.x + j.x * v.y + k.x * v.z + t.x * v.w, i.y * v.x + j.y * v.y + k.y * v.z + t.y * v.w,
				i.z * v.x + j.z * v.y + k.z * v.z + t.z * v.w, i.w * v.x + j.w * v.y + k.w * v.z + t.w * v.w};
	}

	Mat4<T> operator*(CMat4 m) const {
		return {operator*(m.i), operator*(m.j), operator*(m.k), operator*(m.t)};
	}

	Mat4& operator*=(CMat4 m) {
		return *this = operator*(m);
	}

	T* data() {
		return &i.x;
	}
	const T* data() const {
		return &i.x;
	}

	Mat4 inverse() const {
		Mat4<T> r{
			j.y * k.z * t.w - j.y * k.w * t.z - k.y * j.z * t.w + k.y * j.w * t.z + t.y * j.z * k.w - t.y * j.w * k.z,
			-i.y * k.z * t.w + i.y * k.w * t.z + k.y * i.z * t.w - k.y * i.w * t.z - t.y * i.z * k.w + t.y * i.w * k.z,
			i.y * j.z * t.w - i.y * j.w * t.z - j.y * i.z * t.w + j.y * i.w * t.z + t.y * i.z * j.w - t.y * i.w * j.z,
			-i.y * j.z * k.w + i.y * j.w * k.z + j.y * i.z * k.w - j.y * i.w * k.z - k.y * i.z * j.w + k.y * i.w * j.z,

			-j.x * k.z * t.w + j.x * k.w * t.z + k.x * j.z * t.w - k.x * j.w * t.z - t.x * j.z * k.w + t.x * j.w * k.z,
			i.x * k.z * t.w - i.x * k.w * t.z - k.x * i.z * t.w + k.x * i.w * t.z + t.x * i.z * k.w - t.x * i.w * k.z,
			-i.x * j.z * t.w + i.x * j.w * t.z + j.x * i.z * t.w - j.x * i.w * t.z - t.x * i.z * j.w + t.x * i.w * j.z,
			i.x * j.z * k.w - i.x * j.w * k.z - j.x * i.z * k.w + j.x * i.w * k.z + k.x * i.z * j.w - k.x * i.w * j.z,

			j.x * k.y * t.w - j.x * k.w * t.y - k.x * j.y * t.w + k.x * j.w * t.y + t.x * j.y * k.w - t.x * j.w * k.y,
			-i.x * k.y * t.w + i.x * k.w * t.y + k.x * i.y * t.w - k.x * i.w * t.y - t.x * i.y * k.w + t.x * i.w * k.y,
			i.x * j.y * t.w - i.x * j.w * t.y - j.x * i.y * t.w + j.x * i.w * t.y + t.x * i.y * j.w - t.x * i.w * j.y,
			-i.x * j.y * k.w + i.x * j.w * k.y + j.x * i.y * k.w - j.x * i.w * k.y - k.x * i.y * j.w + k.x * i.w * j.y,

			-j.x * k.y * t.z + j.x * k.z * t.y + k.x * j.y * t.z - k.x * j.z * t.y - t.x * j.y * k.z + t.x * j.z * k.y,
			i.x * k.y * t.z - i.x * k.z * t.y - k.x * i.y * t.z + k.x * i.z * t.y + t.x * i.y * k.z - t.x * i.z * k.y,
			-i.x * j.y * t.z + i.x * j.z * t.y + j.x * i.y * t.z - j.x * i.z * t.y - t.x * i.y * j.z + t.x * i.z * j.y,
			i.x * j.y * k.z - i.x * j.z * k.y - j.x * i.y * k.z + j.x * i.z * k.y + k.x * i.y * j.z - k.x * i.z * j.y};

		T c = 1 / (i.x * r.i.x + i.y * r.j.x + i.z * r.k.x + i.w * r.t.x);

		return {r.i * c, r.j * c, r.k * c, r.t * c};
	}

	static Mat4 pitch(float v) {
		return Mat3<T>::pitch(v);
	}
	static Mat4 yaw(float v) {
		return Mat3<T>::yaw(v);
	}
	static Mat4 roll(float v) {
		return Mat3<T>::roll(v);
	}
	static Mat4 translation(CVec3<T> v) {
		return AffineMat4<T>::translation(v);
	}
	static Mat4 rotation(CVec3<T> v) {
		return Mat3<T>::rotation(v);
	}
	static Mat4 scale(CVec3<T> v) {
		return Mat3<T>::scale(v);
	}

	static Mat4 frustum(float left, float right, float bottom, float top, float near, float far) {
		float w = right - left, h = top - bottom, d = far - near, near2 = near * 2;
		// clang-format off
		return Mat4f(near2 / w, 0, 0, 0,
					 0, near2 / h, 0, 0,
					 (right + left) / w, (top + bottom) / h, (far + near) / d, 1,
					 0, 0, -(far * near2) / d, 0);
		// clang-format on
	}

	static Mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
		float w = right - left, h = top - bottom, d = far - near;
		// clang-format off
		return Mat4f(2/w, 0, 0, 0,
					 0, 2/h, 0, 0,
					 0, 0, 2/d, 0,
					 -(right + left) / w, -(top + bottom) / h, -(far + near) / d, 1);
		// clang-format on
	}

	friend std::ostream& operator<<(std::ostream& str, CMat4 m) {
		return str << '(' << m.i << ',' << m.j << ',' << m.k << ',' << m.t << ')';
	}
}; // namespace sgf

} // namespace sgf
