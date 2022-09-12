#include "modelrenderdata.h"

#include "modelrenderer.h"
#include "scene.h"

namespace sgf {

const VertexAttribs defaultModelAttribs{
	AttribFormat::float3, // position
	AttribFormat::float3, // normal
	AttribFormat::none,	  // float4,	//tangent
	AttribFormat::float2, // texCoords0
	AttribFormat::none,	  // float2,	//texCoords1
	AttribFormat::float4, // color
	AttribFormat::none	  // morph
};

namespace {

uint8_t* copyAttrib(uint8_t* dst, const float* src, AttribFormat format) {

	switch (format) {
	case AttribFormat::none:
		return dst;
	case AttribFormat::float1:
		*reinterpret_cast<float*>(dst) = *reinterpret_cast<const float*>(src);
		return dst + 4;
	case AttribFormat::float2:
		*reinterpret_cast<Vec2f*>(dst) = *reinterpret_cast<const Vec2f*>(src);
		return dst + 8;
	case AttribFormat::float3:
		*reinterpret_cast<Vec3f*>(dst) = *reinterpret_cast<const Vec3f*>(src);
		return dst + 12;
	case AttribFormat::float4:
		*reinterpret_cast<Vec4f*>(dst) = *reinterpret_cast<const Vec4f*>(src);
		return dst + 16;
	case AttribFormat::sbyte4:
		dst[0] = src[0] * 127.5f;
		dst[1] = src[1] * 127.5f;
		dst[2] = src[2] * 127.5f;
		dst[3] = src[3] * 127.5f;
		return dst + 4;
	case AttribFormat::ubyte4:
		dst[0] = src[0] * 255.0f;
		dst[1] = src[1] * 255.0f;
		dst[2] = src[2] * 255.0f;
		dst[3] = src[3] * 255.0f;
		return dst + 4;
	case AttribFormat::ubyte2:
		panic("TODO");
	}
	return dst;
}

} // namespace

ModelRenderData* createModelRenderData(const Mesh* mesh, CVertexAttribs attribs) {

	VertexLayout vertexLayout;
	vertexLayout.addAttribs(attribs, 0, 0, 0, 0);
	vertexLayout.indexFormat = IndexFormat::uint32;

	// ***** Create Vertex buffer *****
	//
	uint pitch = bytesPerVertex(attribs);

	Vector<uchar> vertexData(mesh->vertices().size() * pitch);
	{
		auto dst = vertexData.data();

		for (CVertex v : mesh->vertices()) {
			dst = copyAttrib(dst, &v.position.x, attribs[0]);
			dst = copyAttrib(dst, &v.normal.x, attribs[1]);
			dst = copyAttrib(dst, &v.tangent.x, attribs[2]);
			dst = copyAttrib(dst, &v.texCoords0.x, attribs[3]);
			dst = copyAttrib(dst, &v.texCoords1.x, attribs[4]);
			dst = copyAttrib(dst, &v.color.x, attribs[5]);
			dst = copyAttrib(dst, &v.morph.x, attribs[6]);
		}
	}
	auto vertexBuffer =
		graphicsDevice()->createGraphicsBuffer(BufferType::vertex, vertexData.size(), vertexData.data());

	// ***** Create index buffer and surfaces *****
	//
	Vector<uint> indexData(mesh->triangles().size() * 3);
	Vector<ModelRenderData::Surface> opaqueSurfaces;
	Vector<ModelRenderData::Surface> blendedSurfaces;
	uint firstIndex = 0;

	for (auto it = mesh->triangles().begin(); it != mesh->triangles().end();) {

		uint matId = it->materialId;

		auto beg = it;
		for (++it; it != mesh->triangles().end() && matId == it->materialId; ++it) {}

		uint numIndices = (it - beg) * 3;
		uint* dst = indexData.data() + firstIndex;

		for (; beg != it; ++beg) {
			*dst++ = beg->v0;
			*dst++ = beg->v1;
			*dst++ = beg->v2;
		}

		Material* material = (matId < mesh->materials().size() ? mesh->materials()[matId].value() : errorMaterial());
		if(!material) material = defaultMaterial();

		auto& surfaces = (material->blendMode == BlendMode::disable) ? opaqueSurfaces : blendedSurfaces;

		surfaces.emplace_back(material, firstIndex, numIndices);

		firstIndex += numIndices;
	}

	auto indexBuffer =
		graphicsDevice()->createGraphicsBuffer(BufferType::index, indexData.size() * 4, indexData.data());

	// ***** create outline buffer for debugging *****
	//
	Vector<uint> outlineData(mesh->triangles().size() * 6);
	for (uint triId = 0; triId < mesh->triangles().size(); ++triId) {
		auto& tri = mesh->triangles()[triId];
		outlineData[triId * 6 + 0] = tri.v0;
		outlineData[triId * 6 + 1] = tri.v1;
		outlineData[triId * 6 + 2] = tri.v1;
		outlineData[triId * 6 + 3] = tri.v2;
		outlineData[triId * 6 + 4] = tri.v2;
		outlineData[triId * 6 + 5] = tri.v0;
	}
	auto outlineBuffer =
		graphicsDevice()->createGraphicsBuffer(BufferType::index, outlineData.size() * 4, outlineData.data());

	return new ModelRenderData(vertexBuffer, indexBuffer, std::move(opaqueSurfaces), std::move(blendedSurfaces),
					 vertexLayout, outlineBuffer);
}

ModelRenderer* ModelRenderData::getRenderer(Scene* scene) {

	auto it = m_renderers.find(scene);
	if (it != m_renderers.end()) return it->second;

	auto renderer = new ModelRenderer(this);
	scene->addRenderer(renderer);
	m_renderers.insert(std::make_pair(scene, renderer));

	return renderer;
}

} // namespace sgf
