#include "terraindata.h"

#include "material.h"

#include <imgui/imgui.hh>

namespace sgf {

AffineMat4f TerrainData::terrainToLocalMatrix() const {

	Vec3f scale = Vec3f(meshScale, meshScale * heightScale, meshScale);

	Vec3f trans = Vec3f(float(heightMap.value()->width) * .5f, 0, float(heightMap.value()->height) * .5f);

	return AffineMat4f::scale(scale) * AffineMat4f::translation(-trans);
}

AffineMat4f TerrainData::localToTerrainMatrix() const {

	Vec3f scale = Vec3f(meshScale, meshScale * heightScale, meshScale);

	Vec3f trans = Vec3f(float(heightMap.value()->width) * .5f, 0, float(heightMap.value()->height) * .5f);

	return AffineMat4f::translation(trans) * AffineMat4f::scale(1 / scale);
}

float TerrainData::getHeight(CVec3f localPos) const {

	CVec3f tpos = localToTerrainMatrix() * localPos;

	if (tpos.x >= 0 && tpos.z >= 0 && tpos.x < float(heightMap.value()->width) && tpos.z < float(heightMap.value()->height)) {

		//		debug() << "### getHeight" << localPos << tpos << heightMap->getPixel(tpos.x, tpos.z).x;

		return heightMap.value()->getPixel(tpos.x, tpos.z).x * meshScale * heightScale;
	}

	panic("OOPS, you're off the planet!");

	return 0;
}

Boxf TerrainData::localBounds() const {

	const auto& matrix = terrainToLocalMatrix();

	return {matrix * Vec3f(0), matrix * Vec3f(float(heightMap.value()->width), 1, float(heightMap.value()->height))};
}

} // namespace sgf
