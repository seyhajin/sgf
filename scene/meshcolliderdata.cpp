#include "meshcolliderdata.h"

#include "intersect.h"
#include "mesh.h"

namespace sgf {

MeshColliderData::MeshColliderData(const Mesh* mesh) {

	m_vertices.resize(mesh->vertices().size());
	auto vp = m_vertices.data();
	for (const auto& vert : mesh->vertices()) *vp++ = vert.position;

	Vector<Triangle> triangles(mesh->triangles().size());
	auto tp = triangles.data();
	for (const auto& tri : mesh->triangles()) *tp++ = {tri.v0, tri.v1, tri.v2};

	m_rootNode = createNode(std::move(triangles), m_vertices);

	m_bounds = m_rootNode->bounds;

	m_bounds |= boundsPadding;
}

MeshColliderData::Node* MeshColliderData::createNode(Vector<Triangle> triangles, CVector<Vec3f> vertices) {

	Boxf bounds;
	for (const auto& tri : triangles) {
		bounds |= vertices[tri.v0];
		bounds |= vertices[tri.v1];
		bounds |= vertices[tri.v2];
	}

	if (triangles.size() <= 128) { //
		return new Node(bounds, std::move(triangles));
	}

	auto centroid = [&vertices](const Triangle& tri) -> Vec3f {
		return (vertices[tri.v0] + vertices[tri.v1] + vertices[tri.v2]) / 3;
	};

	if (bounds.width() > bounds.height() && bounds.width() > bounds.depth()) {
		std::sort(triangles.begin(), triangles.end(),
				  [centroid](const Triangle& lhs, const Triangle& rhs) { return centroid(lhs).x < centroid(rhs).x; });
	} else if (bounds.height() > bounds.depth()) {
		std::sort(triangles.begin(), triangles.end(),
				  [centroid](const Triangle& lhs, const Triangle& rhs) { return centroid(lhs).y < centroid(rhs).y; });
	} else {
		std::sort(triangles.begin(), triangles.end(),
				  [centroid](const Triangle& lhs, const Triangle& rhs) { return centroid(lhs).z < centroid(rhs).z; });
	}

	auto mid = triangles.size() / 2;

	Node* lhs = createNode(Vector<Triangle>(triangles.begin(), triangles.begin() + mid), vertices);
	Node* rhs = createNode(Vector<Triangle>(triangles.begin() + mid, triangles.end()), vertices);

	return new Node(bounds, lhs, rhs);
}

bool MeshColliderData::intersectRay(CLinef ray, float radius, Contact& contact) const {

	Boxf rayBounds(ray.o);
	rayBounds |= ray * contact.time;
	rayBounds |= radius + boundsPadding;

	return m_rootNode->intersectRay(ray, radius, rayBounds, contact, m_vertices);
}

bool MeshColliderData::Node::intersectRay(CLinef ray, float radius, CBoxf rayBounds, Contact& contact,
										  CVector<Vec3f> vertices) const {

	if (!rayBounds.intersects(bounds)) return false;

	if (triangles.empty()) {
		return lhs->intersectRay(ray, radius, rayBounds, contact, vertices) |
			   rhs->intersectRay(ray, radius, rayBounds, contact, vertices);
	}

	bool collision = false;
	for (const auto& tri : triangles) {

		const auto& v0 = vertices[tri.v0];
		const auto& v1 = vertices[tri.v1];
		const auto& v2 = vertices[tri.v2];

		Boxf triBounds(v0);
		triBounds |= v1;
		triBounds |= v2;

		if (!rayBounds.intersects(triBounds)) continue;

		collision |= intersectRayTriangle(ray, radius, v0, v1, v2, contact);
	}
	return collision;
}

} // namespace sgf
