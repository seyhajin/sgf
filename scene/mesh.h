#pragma once

#include "material.h"

namespace sgf {

struct Vertex {
	Vec3f position;
	Vec3f normal;
	Vec4f tangent;
	Vec2f texCoords0;
	Vec2f texCoords1;
	Vec4f color{1};
	Vec3f morph;

	Vertex() = default;

	Vertex(CVec3f position, CVec3f normal, CVec2f texCoords0 = {}, CVec4f color = {1})
		: position(position), normal(normal), texCoords0(texCoords0), color(color) {
	}

	Vertex(CVec3f position, CVec3f normal, CVec4f tangent, CVec2f texCoords0 = {}, CVec2f texCoords1 = {},
		   CVec4f color = {}, CVec3f morph = {})
		: position(position), normal(normal), tangent(tangent), texCoords0(texCoords0), texCoords1(texCoords1),
		  color(color), morph(morph) {
	}

	bool operator<(const Vertex& that) const {
		if (position != that.position) return position < that.position;
		if (normal != that.normal) return normal < that.normal;
		if (texCoords0 != that.texCoords0) return texCoords0 < that.texCoords0;
		if (texCoords1 != that.texCoords1) return texCoords1 < that.texCoords1;
		return color < that.color;
	}
};
using CVertex = const Vertex&;

struct Triangle {
	union {
		struct {
			uint v0;
			uint v1;
			uint v2;
		};
		uint vertices[3];
	};
	uint materialId;

	Triangle() = default;

	Triangle(uint v0, uint v1, uint v2, uint materialId) : v0(v0), v1(v1), v2(v2), materialId(materialId) {
	}
};
using CTriangle = const Triangle&;

class Mesh : public Shared {
public:
	Mesh() = default;

	Mesh(Material* material) {
		m_materials.push_back(material);
	}

	Mesh(const Mesh* mesh)
		: m_vertices(mesh->m_vertices), m_triangles(mesh->m_triangles), m_materials(mesh->m_materials) {
	}

	Vector<Vertex>& vertices() {
		return m_vertices;
	}

	CVector<Vertex> vertices() const {
		return m_vertices;
	}

	Vector<Triangle>& triangles() {
		return m_triangles;
	}

	CVector<Triangle> triangles() const {
		return m_triangles;
	}

	Vector<SharedPtr<Material>>& materials() {
		return m_materials;
	}

	CVector<SharedPtr<Material>> materials() const {
		return m_materials;
	}

	void addVertex(CVertex vertex) {
		m_vertices.push_back(vertex);
	}

	void addVertex(CVec3f position, CVec3f normal, CVec2f texCoords0) {
		m_vertices.emplace_back(position, normal, texCoords0);
	}

	void addTriangle(CTriangle triangle) {
		m_triangles.push_back(triangle);
	}

	void addTriangle(uint v0, uint v1, uint v2, uint materialId = 0) {
		m_triangles.emplace_back(v0, v1, v2, materialId);
	}

	void addQuad(uint v0, uint v1, uint v2, uint v3, uint materialId = 0) {
		m_triangles.emplace_back(v0, v1, v2, materialId);
		m_triangles.emplace_back(v0, v2, v3, materialId);
	}

	void addMaterial(Material* material) {
		m_materials.push_back(material);
	}

private:
	Vector<Vertex> m_vertices;
	Vector<Triangle> m_triangles;
	Vector<SharedPtr<Material>> m_materials;
};

} // namespace sgf
