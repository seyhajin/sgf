#pragma once

#include <geom/geom.hh>

namespace sgf {

struct MaterialParams {
	// sampler2D baseColorTexture
	// sampler2D metallicRoughnessTexture
	// sampler2D emissiveTexture
	// sampler2D occlusionTexture
	// sampler2D normalTexture
	Vec4f baseColorFactor;
	Vec3f emissiveFactor;
	float metallicFactor;
	float roughnessFactor;
};

} // namespace sgf