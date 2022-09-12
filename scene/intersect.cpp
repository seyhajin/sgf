#include "intersect.h"

#include <cfenv>

namespace sgf {

bool intersectRaySphere(CLinef worldRay, CVec3f v0, float radius, Contact& contact) {

	assert(isUnit(worldRay));

	Linef ray = worldRay - v0;

	float b = -(ray.o.dot(ray.d));
	float c = ray.o.dot(ray.o) - radius * radius;
	float h = b * b - c;
	if (h < 0) return false;

	h = std::sqrt(h);
	float t = b - h;
	if (t < 0 || t >= contact.time) return false;

	Vec3f hp = ray * t;

	contact.point = hp + v0;
	contact.normal = hp / radius;
	contact.time = t;
	//	debug() << "### RaySphere" << Contact.normal.length() << Contact.time;

	return true;
}

bool intersectRayCapsule(CLinef worldRay, CVec3f v0, CVec3f dir, float length, float radius, Contact& contact) {

	assert(isUnit(worldRay) && isUnit(dir));

	Linef ray = worldRay - v0;

	Vec3f po = ray.o - dir * ray.o.dot(dir);
	Vec3f pd = (ray.d - dir * ray.d.dot(dir)).normalized();

	float b = -(po.dot(pd));
	float c = po.dot(po) - radius * radius;
	float h = b * b - c;
	if (h < 0) return false;

	h = std::sqrt(h);
	float t = (b - h);

	Vec3f hp = ray * t;
	float hd = hp.dot(dir);

	if (hd >= 0 && hd <= length) {
		if (t < 0 || t >= contact.time) return false;
		contact.point = hp + v0;
		contact.normal = (po + pd * t) / radius;
		contact.time = t;
		// debug() << "### RayCapsule body" << Contact.normal.length() << Contact.time;

	} else {
		hd = hd > 0 ? length : 0;
		Vec3f org = dir * hd;
		Vec3f v = org - ray.o;
		float b = v.dot(ray.d);
		float c = v.dot(v) - radius * radius;
		float h = b * b - c;
		if (h < 0) return false;

		h = std::sqrt(h);
		float t = (b - h);
		if (t < 0 || t >= contact.time) return false;
		Vec3f hp = ray * t;
		contact.point = hp + v0;
		contact.normal = (hp - org) / radius;
		contact.time = t;
		// debug() << "### RayCapsule cap" << Contact.normal.length() << Contact.time;
	}
	return true;
}

bool intersectRayCylinder(CLinef worldRay, CVec3f v0, CVec3f dir, float length, float radius, Contact& contact) {

	assert(isUnit(worldRay) && isUnit(dir));

	Linef ray = worldRay - v0;

	Vec3f po = ray.o - dir * ray.o.dot(dir);
	Vec3f pd = (ray.d - dir * ray.d.dot(dir)).normalized();

	float b = -(po.dot(pd));
	float c = po.dot(po) - radius * radius;
	float h = b * b - c;
	if (h < 0) return false;

	h = std::sqrt(h);
	float t = (b - h);

	Vec3f hp = ray * t;
	float hd = hp.dot(dir);

	if (hd >= 0 && hd <= length) {
		if (t < 0 || t >= contact.time) return false;
		contact.point = hp + v0;
		contact.normal = (po + pd * t) / radius;
		contact.time = t;
		// debug() << "### RayCylinder body" << Contact.normal.length() << Contact.time;
	} else {
		// TODO: cylinder caps
		return false;
	}

	return true;
}

bool intersectRayEdge(CLinef worldRay, float radius, CVec3f v0, CVec3f v1, Contact& contact) {

	assert(isUnit(worldRay));

	float length = v0.distance(v1);
	Vec3f dir = (v1 - v0) / length;
	Linef ray = worldRay - v0;

	Vec3f po = ray.o - dir * ray.o.dot(dir);
	Vec3f pd = (ray.d - dir * ray.d.dot(dir)).normalized();

	float b = -(po.dot(pd));
	float c = po.dot(po) - radius * radius;
	float h = b * b - c;
	if (h < 0) return false;

	h = std::sqrt(h);
	float t = (b - h);

	Vec3f hp = ray * t;
	float hd = hp.dot(dir);
	if (hd > length) return false;

	if (hd >= 0) {
		if (t < 0 || t >= contact.time) return false;
		contact.point = hp + v0;
		contact.normal = (po + pd * t) / radius;
		contact.time = t;
		//		debug() << "### RayTriangle edge cylinder" << Contact.normal.length() << Contact.time;
	} else {
		// Collide with sphere at v0
		float b = -(ray.o.dot(ray.d));
		float c = ray.o.dot(ray.o) - radius * radius;
		float h = b * b - c;
		if (h < 0) return false;

		h = std::sqrt(h);
		float t = (b - h);
		if (t < 0 || t >= contact.time) return false;

		Vec3f hp = ray * t;

		contact.point = hp + v0;
		contact.normal = hp / radius;
		contact.time = t;
		//		debug() << "### RayTriangle edge sphere" << Contact.normal.length() << Contact.time;
	}

	return true;
}

bool intersectRayTriangle(CLinef ray, float radius, CVec3f v0, CVec3f v1, CVec3f v2, Contact& contact) {

	assert(isUnit(ray));

	Planef plane(v0, v1, v2);
	if (plane.n.dot(ray.d) >= 0) return false;

	plane.d -= radius;
	float t = plane.t_intersect(ray);
	if (t >= contact.time) return false;

	Vec3f hp = ray * t;

	Planef p0(v0 + plane.n, v1, v0);
	bool f0 = p0.distance(hp) >= 0;

	Planef p1(v1 + plane.n, v2, v1);
	bool f1 = p1.distance(hp) >= 0;

	Planef p2(v2 + plane.n, v0, v2);
	bool f2 = p2.distance(hp) >= 0;

	if (t >= 0 && f0 && f1 && f2) {
		contact.point = hp;
		contact.normal = plane.n;
		contact.time = t;
		// debug() << "### RayTriangle plane" << Contact.normal.length() << Contact.time;
		return true;
	}

	bool collision = false;
	if (!f0) collision |= intersectRayEdge(ray, radius, v0, v1, contact);
	if (!f1) collision |= intersectRayEdge(ray, radius, v1, v2, contact);
	if (!f2) collision |= intersectRayEdge(ray, radius, v2, v0, contact);
	return collision;
}

} // namespace sgf
