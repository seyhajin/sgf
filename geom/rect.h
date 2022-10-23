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

	using Vec2 = Vec2<T>;
	using CVec2 = const Vec2&;
	using CRect = const Rect&;

	using Limits = std::numeric_limits<T>;

	Vec2 min{Limits::max(), Limits::max()};

	Vec2 max{Limits::min(), Limits::min()};

	Rect() = default;

	explicit Rect(CVec2 v) : min(v), max(v) {
	}

	Rect(CVec2 min, CVec2 max) : min(min), max(max) {
	}

	Rect(T x0, T y0, T x1, T y1) : min(x0, y0), max(x1, y1) {
	}

	bool empty() const {
		return max.x <= min.x || max.y <= min.y;
	}

	Vec2 center() const {
		return (min + max) / 2;
	}

	Vec2 origin() const {
		return min;
	}

	Vec2 size() const {
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

	T left() const {
		return min.x;
	}

	T right() const {
		return max.x;
	}

	T top() const {
		return min.y;
	}

	T bottom() const {
		return max.y;
	}

	bool contains(CVec2 v) const {
		return v.x >= min.x && v.y >= min.y && v.x < max.x && v.y < max.y;
	}

	bool contains(CRect r) const {
		return min.x <= r.min.x && max.x >= r.max.x && min.y <= r.min.y && max.y >= r.max.y;
	}

	bool intersects(CRect r) const {
		return max.x >= r.min.x && min.x <= r.max.x && max.y >= r.min.y && min.y <= r.max.y;
	}

	Rect operator&(CRect r) const {
		return {min.max(r.min), max.min(r.max)};
	}

	Rect operator|(CRect r) const {
		return {min.min(r.min), max.max(r.max)};
	}

	Rect operator+(CVec2 v) const {
		return {min + v, max + v};
	}

	Rect operator-(CVec2 v) const {
		return {min - v, max - v};
	}

	Rect& operator&=(CRect r) {
		*this = *this & r;
		return *this;
	}

	Rect& operator|=(CRect r) {
		*this = *this | r;
		return *this;
	}

	Rect& operator+=(CVec2 v) {
		*this = *this + v;
		return *this;
	}

	Rect& operator-=(CVec2 v) {
		*this = *this - v;
		return *this;
	}

	bool operator==(CRect r) const {
		return min == r.min && max == r.max;
	}

	bool operator!=(CRect r) const {
		return !*this == r;
	}

	friend std::ostream& operator<<(std::ostream& str, CRect& r) {
		return str << "Rect(" << r.min.x << ',' << r.min.y << ',' << r.max.x << ',' << r.max.y << ')';
	}
};

} // namespace sgf
