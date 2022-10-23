#pragma once

#include <geom/geom.hh>

namespace sgf {

struct alignas(16) TerrainParams {
	//uniform sampler2D terrainHeightTexture;
	//uniform sampler2D terrainNormalTexture;
	//uniform sampler2DArray terrainAerialTexture;
	//uniform sampler2D terrainTopoTexture;
	uint lodLevels;
	uint quadsPerTile;
	Vec2f aerialTextureScale;
	Vec2f aerialTextureOffset;
	Vec2f topoTextureScale;
	Vec2f topoTextureOffset;
	float meshScale;
	float heightScale;
	uint tileMapSize;
};

} // namespace sgf
