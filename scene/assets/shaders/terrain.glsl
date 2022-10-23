uniform sampler2D terrainHeightTexture;
uniform sampler2D terrainNormalTexture;

uniform sampler2DArray terrainAerialTexture;
uniform sampler2D terrainTopoTexture;

struct TerrainParams {
    uint lodLevels;
    uint quadsPerTile;
    vec2 aerialTextureScale;
    vec2 aerialTextureOffset;
    vec2 topoTextureScale;
    vec2 topoTextureOffset;
    float meshScale;
    float heightScale;
    uint  tileMapSize;
};

layout (std140) uniform terrainParams {
    TerrainParams terrain;
};
