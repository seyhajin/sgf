#pragma once

#include "cameraparams.h"
#include "lightparams.h"

namespace sgf {

struct SceneParams {
	Vec4f clearColor;
	Vec4f ambientLightColor;
	Vec4f directionalLightVector;
	Vec4f directionalLightColor;
	uint numLights = 0;
	SGF_ALIGN16 LightParams lights[maxLights];
	float renderTime = 0;
	int debugFlags = 0;
} SGF_ALIGN16;

} // namespace sgf