#pragma once

#include "vec3.h"

#include <core/core.hh>

namespace sgf {

template <class T> struct Line;
template <class T> using CLine = const Line<T>&;
using Linef = Line<float>;
using CLinef = CLine<float>;

template <class T> struct Line {

	Vec3<T> o;
	Vec3<T> d;

	Line() = default;

	Line(CVec3<T> o, CVec3<T> d) : o(o), d(d) {
	}

	Vec3<T> operator*(T t) const {
		return o + d * t;
	}

	Line operator+(CVec3<T> v) const {
		return Line(o + v, d);
	}
	Line operator-(CVec3<T> v) const {
		return Line(o - v, d);
	}

	Line& operator+=(CVec3<T> v) const {
		return *this = operator+(v);
	}
	Line& operator-=(CVec3<T> v) const {
		return *this = operator-(v);
	}

	Line normalized() const {
		return Line(o, d.normalized());
	}

	T t_nearest(CVec3<T> p) const {
		return d.dot(p - o) / d.dot(d);
	}

	Vec3<T> nearest(CVec3<T> p) const {
		return o + d * t_nearest(p);
	}

	friend std::ostream& operator<<(std::ostream& str, CLine<T> l) {
		return str << '(' << l.o << ',' << l.d << ')';
	}
};

} // namespace sgf
