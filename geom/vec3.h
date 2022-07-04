#pragma once

#include "vec2.h"

#include <core/core.hh>

namespace sgf {

template <class T> struct Vec3;
template <class T> using CVec3 = const Vec3<T>&;

using Vec3f = Vec3<float>;
using CVec3f = CVec3<float>;

template <class T> struct Vec3 {

	using CVec3 = const Vec3&;

	T x = 0;
	T y = 0;
	T z = 0;

	constexpr Vec3() = default;

	constexpr Vec3(T s) : x(s), y(s), z(s) {
	}
	constexpr Vec3(T x, T y, T z) : x(x), y(y), z(z) {
	}
	constexpr Vec3(CVec2<T> v, T z) : x(v.x), y(v.y), z(z) {
	}
	template <class C> constexpr Vec3(const Vec3<C>& v) : x(T(v.x)), y(T(v.y)), z(T(v.z)) {
	}

	Vec2<T> xy() const {
		return {x, y};
	}
	Vec2<T> xz() const {
		return {x, z};
	}
	Vec2<T> yz() const {
		return {x, z};
	}

	int compare(CVec3 v) const {
		if (x < v.x) return -1;
		if (v.x < x) return 1;
		if (y < v.y) return -1;
		if (v.y < y) return 1;
		if (z < v.z) return -1;
		if (v.z < z) return 1;
		return 0;
	}

	bool operator<(CVec3 v) const {
		return compare(v) < 0;
	}

	bool operator==(CVec3 v) const {
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator!=(CVec3 v) const {
		return !operator==(v);
	}

	Vec3 operator-() const {
		return {-x, -y, -z};
	}

	Vec3 operator*(CVec3 v) const {
		return {x * v.x, y * v.y, z * v.z};
	}
	Vec3 operator/(CVec3 v) const {
		return {x / v.x, y / v.y, z / v.z};
	}
	Vec3 operator+(CVec3 v) const {
		return {x + v.x, y + v.y, z + v.z};
	}
	Vec3 operator-(CVec3 v) const {
		return {x - v.x, y - v.y, z - v.z};
	}

	Vec3 operator*(T s) const {
		return {x * s, y * s, z * s};
	}
	Vec3 operator/(T s) const {
		return {x / s, y / s, z / s};
	}
	Vec3 operator+(T s) const {
		return {x + s, y + s, z + s};
	}
	Vec3 operator-(T s) const {
		return {x - s, y - s, z - s};
	}
	friend Vec3 operator*(T s, CVec3 v) {
		return {s * v.x, s * v.y, s * v.z};
	}
	friend Vec3 operator/(T s, CVec3 v) {
		return {s / v.x, s / v.y, s / v.z};
	}
	friend Vec3 operator+(T s, CVec3 v) {
		return {s + v.x, s + v.y, s + v.z};
	}
	friend Vec3 operator-(T s, CVec3 v) {
		return {s - v.x, s - v.y, s - v.z};
	}

	Vec3 min(CVec3 v) const {
		return {std::min(x, v.x), std::min(y, v.y), std::min(z, v.z)};
	}
	Vec3 max(CVec3 v) const {
		return {std::max(x, v.x), std::max(y, v.y), std::max(z, v.z)};
	}

	Vec3& operator*=(CVec3 v) {
		return *this = operator*(v);
	}
	Vec3& operator/=(CVec3 v) {
		return *this = operator/(v);
	}
	Vec3& operator+=(CVec3 v) {
		return *this = operator+(v);
	}
	Vec3& operator-=(CVec3 v) {
		return *this = operator-(v);
	}

	Vec3& operator*=(T s) {
		return *this = operator*(s);
	}
	Vec3& operator/=(T s) {
		return *this = operator/(s);
	}
	Vec3& operator+=(T s) {
		return *this = operator+(s);
	}
	Vec3& operator-=(T s) {
		return *this = operator-(s);
	}

	T length() const {
		return sqrt(x * x + y * y + z * z);
	}
	T distance(CVec3 v) const {
		return operator-(v).length();
	}
	T distanceSquared(CVec3 v) const {
		float dx = x - v.x, dy = y - v.y, dz = z - v.z;
		return dx * dx + dy * dy + dz * dz;
	}

	Vec3 normalized() const {
		return operator/(length());
	}
	void normalize() {
		operator/=(length());
	}

	T dot(CVec3 v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	Vec3 cross(CVec3 v) const {
		return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
	}

	T yaw() const {
		return -std::atan2(x, z);
	}

	T pitch() const {
		return -std::atan2(y, sqrtf(x * x + z * z));
	}

	friend std::ostream& operator<<(std::ostream& str, CVec3 v) {
		return str << '(' << v.x << ',' << v.y << ',' << v.z << ')';
	}
};

} // namespace sgf
