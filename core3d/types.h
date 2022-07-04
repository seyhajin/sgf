#pragma once

#include <geom/geom.hh>

namespace sgf {

enum struct BlendMode { disable, alpha, additive, multiply };

enum struct DepthMode { disable, enable, compare };

enum struct CullMode { disable, front, back, all };

enum struct BufferType { uniform, vertex, index };

enum struct TextureFormat {
	alpha8,
	intensity8,
	intensityAlpha16,
	rgb24,
	srgb24,
	rgba32,
	srgba32,
	r32f,
	depth32f,
	rgba64f
};

enum struct TextureFlags {
	none = 0,
	clampS = 1,
	clampT = 2,
	linear = 4,
	mipmap = 8, // also implies linear
	clampST = 3,
};

enum struct AttribFormat { none, float1, float2, float3, float4, sbyte4, ubyte4, ubyte2 };

// AttribSeq?
using VertexAttribs = Vector<AttribFormat>;
using CVertexAttribs = CVector<AttribFormat>;

struct AttribLayout {
	AttribFormat format;
	uint buffer;
	uint location;
	uint offset;
	uint pitch;
	uint divisor;
};

enum struct IndexFormat { none, uint8, uint16, uint32 };

struct VertexLayout {
	Vector<AttribLayout> attribLayouts;
	IndexFormat indexFormat = IndexFormat::none;

	VertexLayout() = default;

	void addAttribs(CVertexAttribs attribs, uint buffer, uint location,
					uint offset, uint divisor);
};
using CVertexLayout = const VertexLayout&;

inline uint channelsPerPixel(TextureFormat format) {
	static constexpr uint r[] = {1, 1, 2, 3, 3, 4, 4, 1, 1, 4};
	return r[uint(format)];
}

inline uint bytesPerChannel(TextureFormat format) {
	static constexpr uint r[] = {1, 1, 1, 1, 1, 1, 1, 4, 4, 2};
	return r[uint(format)];
}

inline uint bytesPerPixel(TextureFormat format) {
	static constexpr uint r[] = {1, 1, 2, 3, 3, 4, 4, 4, 4, 8};
	return r[uint(format)];
}

inline uint channelsPerAttrib(AttribFormat format) {
	static constexpr uint r[] = {0, 1, 2, 3, 4, 4, 4, 2};
	return r[uint(format)];
}

inline uint bytesPerAttrib(AttribFormat format) {
	static constexpr uint r[] = {0, 4, 8, 12, 16, 4, 4, 2};
	return r[uint(format)];
}

inline uint bytesPerVertex(CVertexAttribs attribs) {
	static constexpr uint r[] = {0, 4, 8, 12, 16, 4, 4, 2};
	uint pitch=0;
	for(auto attrib : attribs) pitch+=bytesPerAttrib(attrib);
	return pitch;
}

inline uint bytesPerIndex(IndexFormat format) {
	static constexpr uint r[] = {0, 1, 2, 4};
	return r[uint(format)];
}

} // namespace sgf
