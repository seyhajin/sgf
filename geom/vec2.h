#ifndef VEC2_H
#define VEC2_H

#include <core/core.hh>

namespace sgf {

template <class T> struct Vec2;
template <class T> using CVec2 = const Vec2<T>&;
using Vec2f = Vec2<float>;
using CVec2f = const Vec2f&;
using Vec2i = Vec2<int>;
using CVec2i = const Vec2i&;

template <class T> struct Vec2 {

	using CVec2 = const Vec2&;

	T x = 0;
	T y = 0;

	constexpr Vec2() = default;

	constexpr Vec2(T v) : x{v}, y{v} {
	}
	constexpr Vec2(T x, T y) : x{x}, y{y} {
	}
	template <class C> constexpr Vec2(const Vec2<C>& v) : x(T(v.x)), y(T(v.y)) {
	}

	int compare(CVec2 v) const {
		if (x < v.x) return -1;
		if (v.x < x) return 1;
		if (y < v.y) return -1;
		if (v.y < y) return 1;
		return 0;
	}

	bool operator<(CVec2 v) const {
		return compare(v) < 0;
	}

	Vec2 operator*(CVec2 v) const {
		return {x * v.x, y * v.y};
	}
	Vec2 operator/(CVec2 v) const {
		return {x / v.x, y / v.y};
	}
	Vec2 operator+(CVec2 v) const {
		return {x + v.x, y + v.y};
	}
	Vec2 operator-(CVec2 v) const {
		return {x - v.x, y - v.y};
	}

	Vec2 operator*(float s) const {
		return {x * s, y * s};
	}
	Vec2 operator/(float s) const {
		return {x / s, y / s};
	}
	Vec2 operator+(float s) const {
		return {x + s, y + s};
	}
	Vec2 operator-(float s) const {
		return {x - s, y - s};
	}

	// clang-format off
	Vec2& operator*=(CVec2 v) { x *= v.x; y *= v.y; return *this; }
	Vec2& operator/=(CVec2 v) { x /= v.x; y /= v.y; return *this; }
	Vec2& operator+=(CVec2 v) { x += v.x; y += v.y; return *this;	}
	Vec2& operator-=(CVec2 v) { x -= v.x; y -= v.y; return *this;	}

	Vec2& operator*=(float s) { x *= s; y *= s; return *this; }
	Vec2& operator/=(float s) { x /= s; y /= s; return *this; }
	Vec2& operator+=(float s) { x += s; y += s; return *this; }
	Vec2& operator-=(float s) { x -= s; y -= s; return *this; }
	// clang-format on

	bool operator==(CVec2 v) const {
		return x == v.x && y == v.y;
	}
	bool operator!=(CVec2 v) const {
		return !operator==(v);
	}

	T length() const {
		return std::sqrt(x * x + y * y);
	}
	T distance(CVec2 v) {
		return operator-(v).length();
	}
	Vec2 normalized() const {
		return operator/(length());
	}

	friend std::ostream& operator<<(std::ostream& str, CVec2 v) {
		return str << '(' << v.x << ',' << v.y << ')';
	}
};

} // namespace sgf

#endif // VEC2_H
