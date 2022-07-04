#pragma once

#include "meshutils.h"

namespace sgf{

class MeshBuilder {
public:
	void clear();

	void addTriangle(CVertex v0, CVertex v1, CVertex v2,Material* material);

	Mesh* mesh() const{return m_mesh;}

private:
	SharedPtr<Mesh> m_mesh = new Mesh();

	Map<Vertex, uint> m_vertices;
	Map<SharedPtr<Material>, uint> m_materials;

	uint addMaterial(Material* material);
	uint addVertex(CVertex vertex);
};

}
