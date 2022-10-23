#include "terrainrenderer.h"

#include "shaderasset.h"

#include "assets/shaders/terrainparams.h"

#include <image/image.hh>

namespace sgf {

namespace {

ShaderAsset g_shader("shaders/terrainrenderer.glsl");

struct Vertex {
	Vec3f position;
	Vec3f color;

	Vertex(CVec3f position, CVec3f color) : position(position), color(color) {
	}
};

// VertexFormat vertexFormat({AttribFormat::float3, AttribFormat::float3});

} // namespace

TerrainRenderer::TerrainRenderer(TerrainData* data) : m_data(data) {

	m_data->heightMap.valueChanged.connect(this, [this](Image*) { m_dirty |= Dirty::textures; });
	m_data->normalMap.valueChanged.connect(this, [this](Image*) { m_dirty |= Dirty::textures; });

	m_data->lodLevels.valueChanged.connect(this, [this](uint) { m_dirty |= Dirty::params | Dirty::mesh; });
	m_data->quadsPerTile.valueChanged.connect(this, [this](uint) { m_dirty |= Dirty::params | Dirty::mesh; });

	m_data->aerialTexture.valueChanged.connect(this,[this](TextureResource*) { m_dirty |= Dirty::textures; });
	m_data->aerialTextureScale.valueChanged.connect(this, [this](CVec2f) { m_dirty |= Dirty::params; });
	m_data->aerialTextureOffset.valueChanged.connect(this, [this](CVec2f) { m_dirty |= Dirty::params; });

	m_data->topoTexture.valueChanged.connect(this,[this](TextureResource*) { m_dirty |= Dirty::textures; });
	m_data->topoTextureScale.valueChanged.connect(this, [this](CVec2f) { m_dirty |= Dirty::params; });
	m_data->topoTextureOffset.valueChanged.connect(this, [this](CVec2f) { m_dirty |= Dirty::params; });

	m_data->meshScale.valueChanged.connect(this, [this](float) { m_dirty |= Dirty::params; });
	m_data->heightScale.valueChanged.connect(this, [this](float) { m_dirty |= Dirty::params; });

	m_data->tileMapSize.valueChanged.connect(this,[this](uint){m_dirty |= Dirty::params;});

	m_paramsBuffer = graphicsDevice()->createGraphicsBuffer(BufferType::uniform, sizeof(TerrainParams), nullptr);
	m_dirty = Dirty::all;
}

void TerrainRenderer::validate() {
	if (m_dirty == Dirty::none) return;

	if (bool(m_dirty & Dirty::params)) updateParams();
	if (bool(m_dirty & Dirty::mesh)) updateMesh();

	m_dirty = Dirty::none;
}

void TerrainRenderer::updateParams() {

	TerrainParams params{};

	params.lodLevels = m_data->lodLevels;
	params.quadsPerTile = m_data->quadsPerTile;

	params.aerialTextureScale = m_data->aerialTextureScale;
	params.aerialTextureOffset = m_data->aerialTextureOffset;

	params.topoTextureScale = m_data->topoTextureScale;
	params.topoTextureOffset = m_data->topoTextureOffset;

	params.meshScale = m_data->meshScale;
	params.heightScale = m_data->heightScale;

	params.tileMapSize = m_data->tileMapSize;

	m_paramsBuffer->updateData(0, sizeof(TerrainParams), &params);
}

void TerrainRenderer::updateMesh() {

	m_vertexState = nullptr;

	Vector<Vertex> vertices;
	Vector<uint> indices;

	auto lodLevels = m_data->lodLevels.value();
	auto quadsPerTile = m_data->quadsPerTile.value();

	vertices.reserve(1024 * 1024 * 16);
	indices.reserve(1024 * 1024 * 16);

	for (uint lod = 0; lod < lodLevels; ++lod) {

		//		debug() << "### LOD" << lod;

		auto scale = float(1 << lod);
		auto tileSize = float(quadsPerTile * 4) * scale;

		for (uint v = 0; v < 4; ++v) {

			for (uint h = 0; h < 4; ++h) {

				if (lod && h == 1 && v > 0 && v < 3) h = 3;

				for (uint j = 0; j < quadsPerTile; j += 2) {

					for (uint i = 0; i < quadsPerTile; i += 2) {

						float x = float(h * quadsPerTile + i) * scale - tileSize / 2;
						float y = float(v * quadsPerTile + j) * scale - tileSize / 2;

						static constexpr Vec2f verts[13] = {
							// clang-format off
							Vec2f(0, 0), Vec2f(1, 0), Vec2f(2, 0),
							Vec2f(0, 1), Vec2f(1, 1), Vec2f(2, 1),
							Vec2f(0, 2), Vec2f(1, 2), Vec2f(2, 2),
							Vec2f(.5f, .5f), Vec2f(1.5f, .5f),
							Vec2f(.5f, 1.5f),Vec2f(1.5f, 1.5f)
							// clang-format on
						};

						static constexpr uint tris[48] = {
							// clang-format off
							0,1,9,1,4,9,4,3,9,3,0,9,
							1,2,10,2,5,10,5,4,10,4,1,10,
							3,4,11,4,7,11,7,6,11,6,3,11,
							4,5,12,5,8,12,8,7,12,7,4,12
							// clang-format on
						};

						uint v0 = vertices.size();
						indices.reserve(indices.size() + std::size(tris));
						for (uint k : tris) indices.push_back(k + v0);

						for (CVec2f tv : verts) {
							auto r = rnd();
							auto g = 1 - r;
							Vec3f color;
							switch (lod) {
							case 0:
								color = {r, 0, 0};
								break;
							case 1:
								color = {0, r, 0};
								break;
							case 2:
								color = {0, 0, r};
								break;
							case 3:
								color = {r, r, 0};
								break;
							case 4:
								color = {0, r, r};
								break;
							case 5:
								color = {r, 0, r};
								break;
								//							case 6:
								//								color = {0, 0, r};
								//								break;
							default:
								color = {1, 1, 1};
								break;
							}
							// Vec3f color{1, rnd(1), rnd(.5f)};
							//					vertices.emplace_back(Vec3f(x + tv.x * scale, float(lod), -(y + tv.y *
							// scale)), color);
							vertices.emplace_back(Vec3f(x + tv.x * scale, lod, -(y + tv.y * scale)), color);
						}
					}
				}
			}
		}
	}

	debug() << "### vertices" << vertices.size() << "triangles" << indices.size() / 3;

	m_numIndices = indices.size();

	auto vertexBuffer =
		graphicsDevice()->createGraphicsBuffer(BufferType::vertex, vertices.size() * sizeof(Vertex), vertices.data());

	auto indexBuffer =
		graphicsDevice()->createGraphicsBuffer(BufferType::index, indices.size() * sizeof(uint), indices.data());

	AttribLayout position{AttribFormat::float3, 0, 0, 0, sizeof(Vertex), 0};
	AttribLayout color{AttribFormat::float3, 0, 1, sizeof(Vec3f), sizeof(Vertex), 0};
	VertexLayout vertexLayout{{position, color}, IndexFormat::uint32};

	m_vertexState = graphicsDevice()->createVertexState({vertexBuffer}, indexBuffer, vertexLayout);
}

void TerrainRenderer::bindShaderParams(GraphicsContext* gc) {

	gc->setTexture("terrainHeightTexture", m_heightTexture);
	gc->setTexture("terrainNormalTexture", m_normalTexture);
	gc->setTexture("terrainAerialTexture", m_data->aerialTexture.value());
	gc->setTexture("terrainTopoTexture", m_data->topoTexture.value());

	gc->setUniformBuffer("terrainParams", m_paramsBuffer);
}

Vector<RenderPassType> TerrainRenderer::renderPasses() const {
	return {RenderPassType::opaque};
}

void TerrainRenderer::onUpdate() {
	validate();
}

void TerrainRenderer::onRender(RenderContext& rc, RenderPassType pass) {

	auto gc = rc.graphicsContext();

	gc->setShader(g_shader.open());

	gc->setVertexState(m_vertexState);

	bindShaderParams(gc);

	gc->drawIndexedGeometry(3, 0, m_numIndices, 1);
}

} // namespace sgf
