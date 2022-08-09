#include "meshprimitives.h"

#include "meshutils.h"

namespace sgf {

Mesh* createQuadMesh(float w, float h, Material* material) {

	auto mesh = new Mesh(material);

	float x1 = w / 2, x0 = -x1;
	float y1 = h / 2, y0 = -y1;

	mesh->addVertex(Vertex({x0, y0, 0}, {0, 0, -1}, {0, 0}, {1, 1, 1, 1}));
	mesh->addVertex(Vertex({x1, y0, 0}, {0, 0, -1}, {1, 0}, {1, 1, 1, 1}));
	mesh->addVertex(Vertex({x1, y1, 0}, {0, 0, -1}, {1, 1}, {1, 1, 1, 1}));
	mesh->addVertex(Vertex({x0, y1, 0}, {0, 0, -1}, {0, 1}, {1, 1, 1, 1}));
	mesh->addQuad(0, 1, 2, 3);

	if (material->flatShaded) updateFlatShading(mesh);

	return mesh;
}

Mesh* createBoxMesh(float w, float h, float d, Material* material) {

	auto mesh = new Mesh(material);

	Vec3f verts[] = {{-w / 2, h / 2, -d / 2}, {w / 2, h / 2, -d / 2}, {w / 2, -h / 2, -d / 2}, {-w / 2, -h / 2, -d / 2},
					 {-w / 2, h / 2, d / 2},  {w / 2, h / 2, d / 2},  {w / 2, -h / 2, d / 2},  {-w / 2, -h / 2, d / 2}};

	Vec3f norms[] = {{0, 0, -1}, {1, 0, 0}, {0, 0, 1}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}};

	uint faces[] = {0, 1, 2, 3, 1, 5, 6, 2, 5, 4, 7, 6, 4, 0, 3, 7, 4, 5, 1, 0, 3, 2, 6, 7};

	for (uint i = 0; i < 24; i += 4) {
		CVec3f norm = norms[i / 4];
		mesh->addVertex(Vertex(verts[faces[i + 0]], norm, Vec2f(0, 0)));
		mesh->addVertex(Vertex(verts[faces[i + 1]], norm, Vec2f(1, 0)));
		mesh->addVertex(Vertex(verts[faces[i + 2]], norm, Vec2f(1, 1)));
		mesh->addVertex(Vertex(verts[faces[i + 3]], norm, Vec2f(0, 1)));

		mesh->addQuad(i, i + 1, i + 2, i + 3, 0);
	}

	if (material->flatShaded) updateFlatShading(mesh);

	return mesh;
}

Mesh* createSphereMesh(float radius, int hsegs, int vsegs, Material* material) {

	auto mesh = new Mesh(material);

	// Vertices
	for (int i = 0; i < hsegs; ++i) { //
		mesh->addVertex({{0, radius, 0}, {(i + 0.5f) / hsegs, 0}});
	}
	
	for (int j = 1; j < vsegs; ++j) {
		float pitch = j * pi / vsegs - halfPi;
		for (int i = 0; i <= hsegs; ++i) {
			float yaw = i * twoPi / hsegs;
			auto p = Mat3f::rotation({pitch, yaw, 0}).k * radius;
			mesh->addVertex({p, {float(i) / hsegs, float(j) / vsegs}});
		}
	}
	
	for (int i = 0; i < hsegs; ++i) { //
		mesh->addVertex({{0, -radius, 0}, {(i + 0.5f) / hsegs, 1}}); 
	}

	for(auto& v : mesh->vertices()) v.normal = v.position.normalized();

	// Polygons
	for (int i = 0; i < hsegs; ++i) { //
		mesh->addTriangle(i, i + hsegs + 1, i + hsegs); 
	}
	for (int j = 1; j < vsegs - 1; ++j) {
		for (int i = 0; i < hsegs; ++i) {
			int v0 = j * (hsegs + 1) + i - 1;
			mesh->addQuad(v0, v0 + 1, v0 + hsegs + 2, v0 + hsegs + 1);
		}
	}
	for (int i = 0; i < hsegs; ++i) {
		int v0 = (hsegs + 1) * (vsegs - 1) + i - 1;
		mesh->addTriangle(v0, v0 + 1, v0 + hsegs + 1);
	}

//	if (material->flatShaded) updateFlatShading(mesh);

	return mesh;
}

Mesh* createTorusMesh(float outerRadius, float innerRadius, int outerSegs, int innerSegs, Material* material) {

	auto mesh = new Mesh(material);

	// Vertices
	for (int outer = 0; outer <= outerSegs; ++outer) {

		auto sweep = Mat3f::yaw(outer * twoPi / outerSegs);

		for (int inner = 0; inner <= innerSegs; ++inner) {

			float r = inner * twoPi / innerSegs;
			float c = cosf(r), s = sinf(r);

			auto p = sweep * Vec3f(c * innerRadius + outerRadius, s * innerRadius, 0);
			auto n = sweep * Vec3f(c, s, 0);
			auto t = Vec2f(float(inner) / innerSegs, float(outer) / outerSegs);

			mesh->addVertex({p, n, t});
		}
	}

	// Polygons
	for (int outer = 0; outer < outerSegs; ++outer) {
		uint v0 = outer * (innerSegs + 1);
		for (int inner = 0; inner < innerSegs; ++v0, ++inner) {
			mesh->addQuad(v0, v0 + 1, v0 + innerSegs + 2, v0 + innerSegs + 1);
		}
	}

	if (material->flatShaded) updateFlatShading(mesh);

	return mesh;
}

Mesh* createCylinderMesh(float length, float radius, int segs, Material* material) {

	auto mesh = new Mesh(material);

	float top = length, bot = 0;

	for (int i = 0; i <= segs; ++i) {
		float yaw = (i % segs) * twoPi / segs;
		auto v = Vec3f(cosf(yaw) * radius, top, sinf(yaw) * radius);
		auto n = Vec3f(v.x, 0, v.z).normalized();
		auto t = Vec2f(float(i) / segs, 0);
		mesh->addVertex({v, n, t});
		v.y = bot;
		t.y = 1;
		mesh->addVertex({v, n, t});
	}
	for (int i = 0; i < segs; ++i) { //
		mesh->addQuad(i * 2, i * 2 + 2, i * 2 + 3, i * 2 + 1);
	}

	int v0 = mesh->vertices().size();

	for (int i = 0; i < segs; ++i) {
		float yaw = i * twoPi / segs;
		auto v = Vec3f(cosf(yaw) * radius, top, sinf(yaw) * radius);
		auto n = Vec3f(0, 1, 0);
		auto t = Vec2f(v.x * .5f + .5f, 1 - (v.z * .5f + .5f));
		mesh->addVertex({v, n, t});
		v.y = bot;
		n.y = -n.y;
		mesh->addVertex({v, n, t});
	}
	for (int i = 1; i < segs - 1; ++i) {
		mesh->addTriangle(v0 + (i + 1) * 2, v0 + i * 2, v0);
		mesh->addTriangle(v0 + i * 2 + 1, v0 + (i + 1) * 2 + 1, v0 + 1);
	}

	if (material->flatShaded) updateFlatShading(mesh);

	return mesh;
}

Mesh* createConeMesh(float length, float radius, int segs, bool cap, Material* material) {

	auto mesh = new Mesh(material);

	float top = length, bot = 0;

	for (int i = 0; i < segs; ++i) {
		mesh->addVertex({Vec3f(0, top, 0), Vec3f(0, 1, 0), Vec2f((i + 0.f) / segs, 0), Vec4f(1)});
	}

	for (int i = 0; i <= segs; ++i) {
		float yaw = (i % segs) * twoPi / segs;
		auto n = Vec3f(cosf(yaw), 0, sinf(yaw));
		auto v = Vec3f(n.x * radius, bot, n.z * radius);
		auto t = Vec2f(float(i) / segs, 1);
		mesh->addVertex({v, n, t});
	}

	for (int i = 0; i < segs; ++i) mesh->addTriangle(i, i + segs + 1, i + segs);

	if (cap) {
		uint v0 = mesh->vertices().size();
		for (int i = 0; i < segs; ++i) {
			float yaw = (i % segs) * twoPi / segs;
			auto n = Vec3f(cosf(yaw), 0, sinf(yaw));
			auto v = Vec3f(n.x * radius, bot, n.z * radius);
			auto t = Vec2f(n.x * 0.5f + 0.5f, n.z * 0.5f + 0.5f);
			mesh->addVertex({v, n, t});
		}
		for (int i = 1; i < segs - 1; ++i) mesh->addTriangle(v0, v0 + i, v0 + i + 1);
	}

	if (material->flatShaded) updateFlatShading(mesh);

	return mesh;
}

} // namespace sgf
