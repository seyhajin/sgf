#pragma once

#include "line.h"
#include "vec3.h"

#include <limits>

namespace sgf {

template <class T> struct Box;
template <class T> using CBox = const Box<T>&;
using Boxf = Box<float>;
using CBoxf = CBox<float>;

template <class T> struct Box {

	using L = std::numeric_limits<T>;

	Vec3<T> min{std::numeric_limits<T>::max()};
	Vec3<T> max{std::numeric_limits<T>::lowest()}; // Grrr...lowest instead of min?!?

	Box() = default;

	Box(CVec3<T> min, CVec3<T> max) : min(min), max(max) {
	}
	Box(float radius) : min(-radius), max(radius) {
	}
	Box(CVec3<T> point) : min(point), max(point) {
	}
	Box(CLine<T> line) : Box(line.o) {
		operator|=(line.o + line.d);
	}
	Box(T minx, T miny, T minz, T maxx, T maxy, T maxz) : min(minx, miny, minz), max(maxx, maxy, maxz) {
	}

	T width() const {
		return max.x - min.x;
	}

	T height() const {
		return max.y - min.y;
	}

	T depth() const {
		return max.z - min.z;
	}

	T volume() const {
		return width() * height() * depth();
	}

	T surfaceArea() const {
		return (width() * height() + width() * depth() + height() * depth()) * 2;
	}

	Vec3<T> origin() const {
		return min;
	}

	Vec3<T> center() const {
		return (max + min) * .5f;
	}

	Vec3<T> size() const {
		return max - min;
	}

	float minSize() const {
		return std::min(std::min(width(), height()), depth());
	}

	float maxSize() const {
		return std::max(std::max(width(), height()), depth());
	}

	bool empty() const {
		return max.x <= min.x || max.y <= min.y || max.z <= min.z;
	}

	bool contains(CVec3<T> v) const {
		return v.x >= min.x && v.y >= min.y && v.y >= min.z && v.x < max.x && v.y < max.y && v.z < max.z;
	}

	bool contains(CBox<T> b) const {
		return min.x <= b.min.x && max.x >= b.max.x && min.y <= b.min.y && max.y >= b.max.y && min.z <= b.min.z &&
			   max.z >= b.max.z;
	}

	bool intersects(CBox<T> b) const {
		return max.x >= b.min.x && min.x <= b.max.x && max.y >= b.min.y && min.y <= b.max.y && max.z >= b.min.z &&
			   min.z <= b.max.z;
	}

	Box expanded(float size) const {
		return {min - size, max + size};
	}

	void expand(float size) {
		min -= size;
		max += size;
	}

	bool operator==(CBox<T> b) const {
		return min == b.min && max == b.max;
	}
	bool operator!=(CBox<T> b) const {
		return !operator==(b);
	}

	Box operator+(CVec3<T> v) const {
		return {min + v, max + v};
	}
	Box operator-(CVec3<T> v) const {
		return {min - v, max - v};
	}

	Box operator|(CVec3<T> v) const {
		return {min.min(v), max.max(v)};
	}
	Box operator|(CBox<T> b) const {
		return {min.min(b.min), max.max(b.max)};
	}
	Box operator&(CBox<T> b) const {
		return {min.max(b.min), max.min(b.max)};
	}

	Box& operator+=(CVec3<T> v) {
		return *this = operator+(v);
	}
	Box& operator-=(CVec3<T> v) {
		return *this = operator-(v);
	}

	Box& operator|=(CVec3<T> v) {
		return *this = operator|(v);
	}
	Box& operator|=(CBox<T> b) {
		return *this = operator|(b);
	}
	Box& operator&=(CBox<T> b) {
		return *this = operator&(b);
	}

	Vec3<T> corner(int id) const {
		return {(id & 1) ? max.x : min.x, (id & 2) ? max.y : min.y, (id & 4) ? max.z : min.z};
	}

	friend std::ostream& operator<<(std::ostream& str, CBox<T> b) {
		return str << '(' << b.min << ',' << b.max << ')';
	}
};

} // namespace sgf
