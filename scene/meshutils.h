#pragma once

#include "mesh.h"

namespace sgf {

void flipMesh(Mesh* mesh);

Boxf meshBounds(const Mesh* mesh);

void fitMesh(Mesh* mesh, CBoxf bounds, bool uniform);

void transformMesh(Mesh* mesh, CAffineMat4f matrix);

void transformMesh(Mesh* mesh, CMat4f matrix);

void transformMesh(Mesh* mesh, CMat3f matrix);

void translateMesh(Mesh* mesh, CVec3f translation);

void rotateMesh(Mesh* mesh, CVec3f rotation);

void scaleMesh(Mesh* mesh, CVec3f scale);

void addMesh(const Mesh* srcMesh, Mesh* dstMesh);

void updateFlatShading(Mesh* mesh);

void mergeNearVertices(Mesh* mesh, float epsilon);

void removeDegenerateTriangles(Mesh* mesh);

void removeDuplicateTriangles(Mesh* mesh);

void splitMesh(Mesh* mesh, CPlanef plane, Mesh* front, Mesh* back);

} // namespace sgf
