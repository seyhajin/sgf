#pragma once

#include "collider.h"

namespace sgf {

class Mesh;

class MeshColliderData : public Shared {
public:
	static constexpr float boundsPadding = .01f;

	MeshColliderData(const Mesh *mesh);

	bool intersectRay(CLinef ray, float radius, Contact& contact) const;

	Boxf bounds() const {
		return m_bounds;
	}

private:
	friend class MeshCollider;

	struct Triangle {
		uint v0, v1, v2;
	};

	struct Node {
		Boxf const bounds;
		Node* const lhs = nullptr;
		Node* const rhs = nullptr;
		Vector<Triangle> const triangles;

		Node(CBoxf bounds, Vector<Triangle> triangles) : bounds(bounds), triangles(std::move(triangles)) {
		}

		Node(CBoxf bounds, Node* lhs, Node* rhs) : bounds(bounds), lhs(lhs), rhs(rhs) {
		}

		bool intersectRay(CLinef ray, float radius, CBoxf rayBounds, Contact& contact, CVector<Vec3f> vertices) const;
	};

	Vector<Vec3f> m_vertices;
	Node* m_rootNode = nullptr;
	Boxf m_bounds;

	static Node* createNode(Vector<Triangle> triangles, CVector<Vec3f> vertices);
};

MeshColliderData* createMeshColliderData(Mesh* mesh);


}
