#pragma once

#include <geom/geom.hh>

namespace sgf {

struct alignas(16) MaterialParams {
	// sampler2D baseColorTexture
	// sampler2D metallicRoughnessTexture
	// sampler2D emissiveTexture
	// sampler2D occlusionTexture
	// sampler2D normalTexture
	Vec4f baseColorFactor;
	Vec3f emissiveFactor;
	float metallicFactor{0};
	float roughnessFactor{0};
};

} // namespace sgf
