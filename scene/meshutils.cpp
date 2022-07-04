#include "meshutils.h"

#include "meshbuilder.h"

namespace sgf {

void flipMesh(Mesh* mesh) {
	for (auto& vert : mesh->vertices()) { vert.normal = -vert.normal; }
	for (auto& tri : mesh->triangles()) std::swap(tri.v1, tri.v2);
}

void transformMesh(Mesh* mesh, CMat4f matrix) {

	auto cofactor = Mat3f(matrix.i.x, matrix.i.y, matrix.i.z, matrix.j.x, matrix.j.y, matrix.j.z, matrix.k.x,
						  matrix.k.y, matrix.k.z)
						.cofactor();

	for (auto& v : mesh->vertices()) {
		v.position = (matrix * Vec4f(v.position, 1)).xyz();
		v.normal = (cofactor * v.normal).normalized();
		v.morph = (matrix * Vec4f(v.morph, 1)).xyz();
	}
}

void transformMesh(Mesh* mesh, CAffineMat4f matrix) {

	auto cofactor = matrix.m.cofactor();

	for (auto& v : mesh->vertices()) {
		v.position = matrix * v.position;
		v.normal = (cofactor * v.normal).normalized();
		v.morph = matrix * v.morph;
	}
}

void transformMesh(Mesh* mesh, CMat3f matrix) {

	auto cofactor = matrix.cofactor();

	for (auto& v : mesh->vertices()) {
		v.normal = (cofactor * v.normal).normalized();
		v.morph = matrix * v.morph;
	}
}

void translateMesh(Mesh* mesh, CVec3f translation) {
	for (auto& v : mesh->vertices()) { //
		v.position += translation;
	}
}

void rotateMesh(Mesh* mesh, CVec3f rotation) {

	auto matrix = Mat3f::rotation(rotation);

	for (auto& v : mesh->vertices()) {
		v.normal = matrix * v.normal;
		v.morph = matrix * v.morph;
	}
}

void scaleMesh(Mesh* mesh, CVec3f scale) {
	transformMesh(mesh, Mat3f::scale(scale));
}

void addMesh(const Mesh* src, Mesh* dst) {

	auto& dstVerts = dst->vertices();
	const auto& srcVerts = src->vertices();

	uint v0 = dst->vertices().size();
	dstVerts.insert(dstVerts.end(), srcVerts.begin(), srcVerts.end());

	Vector<uint> matIds;
	matIds.reserve(src->materials().size());

	for (uint i = 0; i < src->materials().size(); ++i) {
		bool found = false;
		for (uint j = 0; j < dst->materials().size(); ++j) {
			if (src->materials()[i] != dst->materials()[j]) continue;
			matIds.push_back(j);
			found = true;
			break;
		}
		if (found) continue;
		dst->addMaterial(src->materials()[i]);
		matIds.push_back(dst->materials().size() - 1);
	}

	for (auto& tri : src->triangles()) {
		dst->addTriangle(tri.v0 + v0, tri.v1 + v0, tri.v2 + v0, matIds[tri.materialId]);
	}
}

void updateFlatShading(Mesh* mesh) {

	auto& vertices = mesh->vertices();

	Vector<bool> provoking(vertices.size());

	//		int n_provoking = 0;
	for (auto& tri : mesh->triangles()) {

		uint v0 = tri.v0;
		uint v1 = tri.v1;
		uint v2 = tri.v2;

		// Find a provoking vertex
		if (!provoking[v0]) {
			provoking[v0] = true;
		} else if (!provoking[v1]) {
			provoking[v1] = true;
			tri.v0 = v1;
			tri.v1 = v2;
			tri.v2 = v0;
		} else if (!provoking[v2]) {
			provoking[v2] = true;
			tri.v0 = v2;
			tri.v1 = v0;
			tri.v2 = v1;
		} else {
			mesh->addVertex(vertices[tri.v0]);
			tri.v0 = mesh->vertices().size() - 1;
		}

		Planef plane(vertices[v0].position, vertices[v1].position, vertices[v2].position);

		Vertex& v = vertices[tri.v0];
		v.color = Vec4f(1);
		v.normal = plane.n;
	}
	// if (n_provoking != 0) debug() << "Provoking" << n_provoking;
}

Boxf meshBounds(const Mesh* mesh) {
	Boxf bounds;
	for (auto& v : mesh->vertices()) bounds |= v.position;
	return bounds;
}

void fitMesh(Mesh* mesh, CBoxf box, bool uniform) {

	auto bounds = meshBounds(mesh);

	auto scale = box.size() / bounds.size();
	if (uniform) scale = Vec3f(std::min(scale.x, std::min(scale.y, scale.z)));

	auto mat = Mat3f::scale(scale);
	auto vec = box.center() - mat * bounds.center();

	transformMesh(mesh, {mat, vec});
}

void mergeNearVertices(Mesh* mesh, float epsilon) {

	struct CmpFunc {
		float epsilon;
		CmpFunc(float epsilon) : epsilon(epsilon) {
		}
		bool operator()(CVec3f p, CVec3f q) const {
			if (std::abs(p.x - q.x) > epsilon) return p.x < q.x;
			if (std::abs(p.y - q.y) > epsilon) return p.y < q.y;
			if (std::abs(p.z - q.z) > epsilon) return p.z < q.z;
			return 0;
		}
	};

	std::map<Vec3f, uint, CmpFunc> vertices{CmpFunc(epsilon)};
	Vector<uint> vertIds(mesh->vertices().size());

	for (uint vertId = 0; vertId < mesh->vertices().size(); ++vertId) {
		auto& vert = mesh->vertices()[vertId];
		auto it = vertices.find(vert.position);
		if (it == vertices.end()) {
			vertices.insert(std::make_pair(vert.position, vertId));
			vertIds[vertId] = vertId;
			continue;
		}
		vertIds[vertId] = it->second;
	}

	for (auto& tri : mesh->triangles()) {
		tri.v0 = vertIds[tri.v0];
		tri.v1 = vertIds[tri.v1];
		tri.v2 = vertIds[tri.v2];
	}
}

void removeDegenerateTriangles(Mesh* mesh) {

	uint put = 0;
	for (uint triId = 0; triId < mesh->triangles().size(); ++triId) {
		auto& tri = mesh->triangles()[triId];
		if (tri.v0 == tri.v1 || tri.v0 == tri.v2 || tri.v1 == tri.v2) continue;
		mesh->triangles()[put++] = tri;
	}
	if (put < mesh->triangles().size()) {
		debug() << "### removeDegenerateTriangles removed" << (mesh->triangles().size() - put) << "triangles";
		mesh->triangles().resize(put);
	}
}

void removeDuplicateTriangles(Mesh* mesh) {

	struct CompareTris {
		ulong hash(const Triangle& tri) const {
			ulong v0 = tri.v0;
			ulong v1 = tri.v1;
			ulong v2 = tri.v2;
			assert(v0 != v1 && v0 != v2 && v1 != v2);
			if (v1 < v0) std::swap(v0, v1);
			if (v2 < v0) std::swap(v0, v2);
			if (v2 < v1) std::swap(v1, v2);
			return (v0 << 42) | (v1 << 21) | v2;
		}

		bool operator()(const Triangle& x, const Triangle& y) const {
			return hash(x) < hash(y);
		}
	};

	std::map<Triangle, uint, CompareTris> triMap;

	uint put = 0;
	for (uint triId = 0; triId < mesh->triangles().size(); ++triId) {
		auto& tri = mesh->triangles()[triId];
		auto it = triMap.find(tri);
		if (it != triMap.end()) continue;
		triMap.insert(std::make_pair(tri, put));
		mesh->triangles()[put++] = tri;
	}
	if (put < mesh->triangles().size()) {
		debug() << "### removeDuplicateTriangles removed" << (mesh->triangles().size() - put) << "triangles";
		mesh->triangles().resize(put);
	}
}

void splitMesh(Mesh* mesh, CPlanef plane, Mesh* front, Mesh* back) {

	float eps = .001f;

	MeshBuilder meshBuilders[2];
	Vector<Vertex> triVerts[2];

	for (auto tri : mesh->triangles()) {

		auto material = mesh->materials()[tri.materialId];

		Vertex verts[3];
		float dists[3];
		uint fr = 0;
		uint bk = 0;

		for (uint i = 0; i < 3; ++i) {

			verts[i] = mesh->vertices()[tri.vertices[i]];
			dists[i] = plane.distance(verts[i].position);

			if (dists[i] > eps) {
				++fr;
			} else if (dists[i] < -eps) {
				++bk;
			} else {
				dists[i] = 0;
			}
		}

		if (!fr || !bk) {
			uint side;
			if (!fr && !bk) {
				side = Plane(verts[0].position, verts[1].position, verts[2].position).n.dot(plane.n) >= 0;
			} else {
				side = fr > 0;
			}
			meshBuilders[side].addTriangle(verts[0], verts[1], verts[2], material);
			continue;
		}

		uint i0;
		for (i0 = 0; i0 < 3 && dists[i0] == 0; ++i0) {}
		assert(i0 < 3);

		// Vertex 0 easy!
		uint side = dists[i0] > 0;
		triVerts[side].push_back(verts[i0]);

		// Vertex 1, 2...
		for (uint j = 1; j < 3; ++j) {

			uint prev = (i0 + j + 2) % 3;
			uint curr = (i0 + j) % 3;

			CVertex cv = verts[curr];
			float pd = dists[prev];
			float cd = dists[curr];

			// Crosses plane?
			if ((cd > 0 && pd < 0) || (cd < 0 && pd > 0)) {

				// Calc intersection point
				float t = cd / (cd - pd);

				CVertex pv = verts[prev];
				Vertex iv = cv;

				iv.position = (cv.position - pv.position) * t + pv.position;
				iv.normal = ((cv.normal - pv.normal) * t + pv.normal).normalized();
				iv.texCoords0 = (cv.texCoords0 - pv.texCoords0) * t + pv.texCoords0;
				iv.texCoords1 = (cv.texCoords1 - pv.texCoords1) * t + pv.texCoords1;
				iv.color = (cv.color - pv.color) * t + pv.color;

				// Intersection point added to both sides
				triVerts[1 - side].push_back(iv);
				triVerts[side].push_back(iv);

				side = 1 - side;
			}
			triVerts[side].push_back(cv);
		}

		for (uint j = 0; j < 2; ++j) {
			auto& tv = triVerts[j];
			if (tv.empty()) continue;
			assert(tv.size() == 3);
			meshBuilders[j].addTriangle(tv[0], tv[1], tv[2], material);
		}
	}

	if (front) addMesh(meshBuilders[1].mesh(), front);

	if (back) addMesh(meshBuilders[0].mesh(), back);
}

} // namespace sgf
