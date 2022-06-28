#pragma once

#include "vec2.h"

#include <core/core.hh>
#include <limits>

namespace sgf {

template <class T> struct Rect;
template <class T> using CRect = const Rect<T>&;
using Recti = Rect<int>;
using CRecti = CRect<int>;
using Rectf = Rect<float>;
using CRectf = CRect<float>;

template <class T> struct Rect {

	using Limits = std::numeric_limits<T>;

	Vec2<T> min{Limits::max(), Limits::max()};

	Vec2<T> max{Limits::min(), Limits::min()};

	Rect() = default;

	explicit Rect(CVec2<T> v) : min(v), max(v) {
	}

	Rect(CVec2<T> min, CVec2<T> max) : min(min), max(max) {
	}

	Rect(T x0, T y0, T x1, T y1) : min(x0, y0), max(x1, y1) {
	}

	bool empty() const {
		return max.x <= min.x || max.y <= min.y;
	}

	Vec2<T> origin() const {
		return min;
	}

	Vec2<T> size() const {
		return Vec2(max.x - min.x, max.y - min.y);
	}

	T x() const {
		return min.x;
	}

	T y() const {
		return min.y;
	}

	T width() const {
		return max.x - min.x;
	}

	T height() const {
		return max.y - min.y;
	}

	bool contains(CVec2<T> v) const {
		return v.x >= min.x && v.y >= min.y && v.z < max.z;
	}

	bool contains(CRect<T> r) const {
		return min.x <= r.min.x && max.x >= r.max.x && min.y <= r.min.y && max.y >= r.max.y;
	}

	bool intersects(CRect<T> r) const {
		return max.x >= r.min.x && min.x <= r.max.x && max.y >= r.min.y && min.y <= r.max.y;
	}

	Rect operator+(CVec2<T> v) const {
		return Rect{min + v, max + v};
	}

	Rect operator-(CVec2<T> v) const {
		return Rect{min - v, max - v};
	}

	Rect& operator+=(CVec2<T> v) {
		min += v;
		max += v;
		return *this;
	}

	Rect& operator-=(CVec2<T> v) {
		min -= v;
		max -= v;
		return *this;
	}

	bool operator==(CRect<T> r) const {
		return min == r.min && max == r.max;
	}

	bool operator!=(CRect<T> r) const {
		return !operator==(r);
	}

	friend std::ostream& operator<<(std::ostream& str, CRect<T>& r) {
		return str << "Rect(" << r.min.x << ',' << r.min.y << ',' << r.max.x << ',' << r.max.y << ')';
	}
};

} // namespace sgf
