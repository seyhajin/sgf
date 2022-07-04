#pragma once

#include "mesh.h"

namespace sgf {

Mesh* createQuadMesh(float width, float height, Material* material = nullptr);

Mesh* createBoxMesh(float width, float height, float depth, Material* material = nullptr);

Mesh* createSphereMesh(float radius, int xSegs, int ySegs, Material* material = nullptr);

Mesh* createCylinderMesh(float length, float radius, int segs, Material* material = nullptr);

Mesh* createConeMesh(float length, float radius, int segs, bool cap, Material* material = nullptr);

Mesh* createTorusMesh(float outerRadius, float innerRadius, int outerSegs, int innerSegs, Material* material = nullptr);

} // namespace sgf
