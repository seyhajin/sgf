#pragma once

#include "cameraparams.h"
#include "lightparams.h"

namespace sgf {

struct SceneParams {
	Vec4f clearColor;
	Vec4f ambientLightColor;
	Vec4f directionalLightVector;
	Vec4f directionalLightColor;
	LightParams lights[maxLights];
	float renderTime = 0;
	int debugFlags = 0;
	int numLights = 0;
	char _pad[4];
};

SGF_ASSERT_ALIGNED16(SceneParams);

} // namespace sgf
