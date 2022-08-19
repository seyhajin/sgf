#pragma once

#include <core/core.hh>

#include "vec2.h"
#include "vec3.h"

namespace sgf {

template <class T> struct Vec4;
template <class T> using CVec4 = const Vec4<T>&;
using Vec4f = Vec4<float>;
using CVec4f = CVec4<float>;

template <class T> struct Vec4 {
	using CVec4 = const Vec4&;

	T x = 0;
	T y = 0;
	T z = 0;
	T w = 0;

	Vec4() = default;

	Vec4(T s) : x(s), y(s), z(s), w(s) {
	}

	Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {
	}

	Vec4(CVec2<T> v, T z, T w) : x(v.x), y(v.y), z(z), w(w) {
	}

	Vec4(CVec3<T> v, T w) : x(v.x), y(v.y), z(v.z), w(w) {
	}

	template <class C> constexpr Vec4(const Vec4<C>& v) : x(T(v.x)), y(T(v.y)), z(T(v.z)), w(T(v.w)) {
	}

	int compare(CVec4 v) const {
		if (x < v.x) return -1;
		if (v.x < x) return 1;
		if (y < v.y) return -1;
		if (v.y < y) return 1;
		if (z < v.z) return -1;
		if (v.z < z) return 1;
		if (w < v.w) return -1;
		if (v.w < w) return 1;
		return 0;
	}

	bool operator<(CVec4 v) const {
		return compare(v) < 0;
	}

	CVec3<T> xyz() const {
		return (CVec3<T>)(*this);
	}

	Vec4 operator-() const {
		return {-x, -y, -z, -w};
	};

	Vec4 operator*(CVec4 v) const {
		return {x * v.x, y * v.y, z * v.z, w * v.w};
	}
	Vec4 operator/(CVec4 v) const {
		return {x / v.x, y / v.y, z / v.z, w / v.w};
	}
	Vec4 operator+(CVec4 v) const {
		return {x + v.x, y + v.y, z + v.z, w + v.w};
	}
	Vec4 operator-(CVec4 v) const {
		return {x - v.x, y - v.y, z - v.z, w - v.w};
	}

	Vec4 operator*(T s) const {
		return {x * s, y * s, z * s, w * s};
	}
	Vec4 operator/(T s) const {
		return {x / s, y / s, z / s, w / s};
	}
	Vec4 operator+(T s) const {
		return {x + s, y + s, z + s, w + s};
	}
	Vec4 operator-(T s) const {
		return {x - s, y - s, z - s, w - s};
	}

	// clang-format off
	Vec4& operator*=(CVec4 v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
	Vec4& operator/=(CVec4 v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }
	Vec4& operator+=(CVec4 v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	Vec4& operator-=(CVec4 v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }

	Vec4& operator*=(T s) { x *= s; y *= s; z *= s; w *= s; return *this; }
	Vec4& operator/=(T s) { x /= s; y /= s; z /= s; w /= s; return *this; }
	Vec4& operator+=(T s) { x += s; y += s; z += s; w += s; return *this; }
	Vec4& operator-=(T s) { x -= s; y -= s; z -= s; w -= s; return *this; }

	bool operator==(CVec4 that)const{return x==that.x && y==that.y && z==that.z && w==that.w;}
	bool operator!=(CVec4 that)const{return !operator==(that);}

	uint rgba()const{
		constexpr T sc=T(255);
		return (uint(w * sc) << 24) | (uint(z * sc) << 16) | (uint(y * sc) << 8) | uint(x * sc);
	}

	static Vec4 rgba(uint rgba) {
		constexpr T sc = T(1)/T(255);
		return  {T(rgba & 255) * sc, T((rgba >> 8) & 255) * sc, T((rgba >> 16) & 255) * sc, T(rgba >> 24) * sc};
	}

	// clang-format on

	friend std::ostream& operator<<(std::ostream& str, CVec4& v) {
		return str << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')';
	}
};

} // namespace sgf
