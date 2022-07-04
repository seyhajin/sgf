#include "meshbuilder.h"

namespace sgf{

void MeshBuilder::clear() {
	m_mesh=new Mesh();
	m_vertices.clear();
	m_materials.clear();
}

void MeshBuilder::addTriangle(CVertex v0, CVertex v1, CVertex v2,Material* material) {
	Triangle tri;
	tri.v0 = addVertex(v0);
	tri.v1 = addVertex(v1);
	tri.v2 = addVertex(v2);
	tri.materialId=addMaterial(material);
	m_mesh->addTriangle(tri);
}

uint MeshBuilder::addVertex(CVertex vertex) {

	const auto& it = m_vertices.find(vertex);
	if (it != m_vertices.end()) return it->second;

	uint id = m_mesh->vertices().size();
	m_mesh->vertices().push_back(vertex);
	m_vertices.insert(std::make_pair(vertex, id));

	return id;
}

uint MeshBuilder::addMaterial(Material* material) {
	assert(material);

	auto it = m_materials.find(material);
	if (it != m_materials.end()) return it->second;

	uint id = m_mesh->materials().size();
	m_mesh->addMaterial(material);
	m_materials.insert(std::make_pair(material, id));

	return id;
}

}
