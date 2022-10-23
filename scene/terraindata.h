#pragma once

#include "../scene/renderer.h"

#include <image/image.h>

namespace sgf {

class TerrainData : public Shared {
public:
	SGF_OBJECT_TYPE(TerrainData, Shared);

	TerrainData() = default;
	~TerrainData() override = default;

	Property<SharedPtr<Image>> heightMap;
	Property<SharedPtr<Image>> normalMap;

	Property<uint> lodLevels;
	Property<uint> quadsPerTile;	// TODO: Should just be quadsPerLod?

	Property<SharedPtr<TextureResource>> aerialTexture;
	Property<Vec2f> aerialTextureScale{1};
	Property<Vec2f> aerialTextureOffset{0};

	Property<SharedPtr<TextureResource>> topoTexture;
	Property<Vec2f> topoTextureScale{1};
	Property<Vec2f> topoTextureOffset{0};

	Property<float> meshScale{1};
	Property<float> heightScale{1};

	Property<uint> tileMapSize;

	float terrainSize() const {
		return float(1 << (lodLevels-1)) * float(quadsPerTile * 4);
	}

	// matrix to convert terrain coords 0...heightMap width/height to local space coords
	AffineMat4f terrainToLocalMatrix() const;

	AffineMat4f localToTerrainMatrix() const;

	// ignores localPos.y coordinate
	float getHeight(CVec3f localPos) const;

	float getHeight(float x, float z) const { return getHeight(Vec3f(x, 0, z)); }

	Boxf localBounds() const;
};

} // namespace wb
