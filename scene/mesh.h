#pragma once

#include "material.h"

namespace sgf {

struct Vertex {
	Vec3f position;
	Vec3f normal;
	Vec4f tangent;
	Vec2f texCoords;
	Vec4f color;

	Vertex(CVec3f position = {}, CVec3f normal = {}, CVec4f tangent = {}, CVec2f texCoords = {}, CVec4f color = {})
		: position(position), normal(normal), tangent(tangent), texCoords(texCoords), color(color) {
	}
};

struct Triangle {
	uint v0;
	uint v1;
	uint v2;
	uint material;
};

struct TriMesh {
	Vector<Vertex> vertices;
	Vector<Triangle> triangle;
	Vector<SharedPtr<Material>> materials;
};

} // namespace sgf
