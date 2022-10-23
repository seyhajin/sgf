#pragma once

#include <geom/geom.hh>

namespace sgf {

struct alignas(16) EnvParams {
	//sampler2D envSkyTexture
	Vec4f fogColor{1};
	float fogRange{0};
	float fogPower{1};
};

}
