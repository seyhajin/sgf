#pragma once

#include "assets/shaders/cameraparams.h"
#include "assets/shaders/lightparams.h"
#include "assets/shaders/materialparams.h"
#include "assets/shaders/sceneparams.h"

namespace sgf {

struct RenderParams {
	CameraParams camera;
	SceneParams scene;
};

} // namespace sgf
