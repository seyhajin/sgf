#pragma once

#include "mesh.h"
#include "model.h"

namespace sgf {

Mesh* createBoxMesh(CBoxf box, Material* material);
Mesh* createBoxMesh(float width, float height, float depth, Material* material);
Mesh* createSphereMesh(float radius, int xSegs, int ySegs, Material* material);
Mesh* createCylinderMesh(float length, float radius, int segs, Material* material);
Mesh* createConeMesh(float length, float radius, int segs, bool cap, Material* material);
Mesh* createTorusMesh(float outerRadius, float innerRadius, int outerSegs, int innerSegs, Material* material);

Model* createModel(Mesh* mesh, bool collider);
Model* createBoxModel(CBoxf box, Material* material, bool collider);
Model* createBoxModel(float width, float height, float depth, Material* material, bool collider);
Model* createSphereModel(float radius, int xSegs, int ySegs, Material* material, bool collider);
Model* createCylinderModel(float length, float radius, int segs, Material* material, bool collider);
Model* createConeModel(float length, float radius, int segs, bool capped, Material* material, bool collider);
Model* createTorusModel(float outerRadius, float innerRadius, int outerSegs, int innerSegs, Material* material,
						bool collider);

} // namespace sgf
