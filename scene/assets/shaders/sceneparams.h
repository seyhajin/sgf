#pragma once

#include "cameraparams.h"
#include "lightparams.h"

namespace sgf {

struct alignas(16) SceneParams {
	Vec4f clearColor;
	Vec4f ambientLightColor;
	Vec4f directionalLightVector;
	Vec4f directionalLightColor;
	LightParams lights[maxLights];
	float renderTime{0};
	int debugFlags{0};
	int numLights{0};
};

} // namespace sgf
