#pragma once

#include <core/core.hh>

#include "line.h"
#include "vec3.h"

namespace sgf {

template <class T> struct Plane;
template <class T> using CPlane = const Plane<T>&;
using Planef = Plane<float>;
using CPlanef = CPlane<float>;

template <class T> struct Plane {

	// One tenth of a mm.
	static constexpr float degenerateNormalLength = .0001f;

	Vec3<T> n;
	T d = 0;

	Plane() = default;

	Plane(CVec3<T> n, T d) : n(n), d(d) {
		assert(!degenerate());
	}

	Plane(CVec3<T> p, CVec3<T> n) : n(n), d(-n.dot(p)) {
		assert(!degenerate());
	}

	Plane(CVec3<T> v0, CVec3<T> v1, CVec3<T> v2) : n((v1 - v0).cross(v2 - v0)), d(-n.dot(v0)) {
		assert(!degenerate());
		this->n.normalize();
	}

	bool degenerate() const {
		return std::abs(n.length()) < degenerateNormalLength;
	}

	bool operator==(CPlane<T>& that) const {
		return n == that.n && d == that.d;
	}
	bool operator!=(CPlane<T>& that) const {
		return !operator==(that);
	}

	T distance(CVec3<T> p) const {
		return n.dot(p) + d;
	}

	Vec3<T> nearest(CVec3<T> p) const {
		return p - n * distance(p);
	}

	T t_intersect(CLine<T>& line) const {
		return -distance(line.o) / n.dot(line.d);
	}

	Vec3<T> intersect(CLine<T>& line) const {
		return line * t_intersect(line);
	}

	friend std::ostream& operator<<(std::ostream& str, CPlane<T> p) {
		return str << '(' << p.n << ',' << p.d << ')';
	}
};

} // namespace sgf
