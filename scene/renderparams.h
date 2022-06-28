#pragma once

#include "shaders/cameraparams.h"
#include "shaders/lightparams.h"
#include "shaders/materialparams.h"
#include "shaders/sceneparams.h"

namespace sgf {

struct RenderParams {
	CameraParams camera;
	SceneParams scene;
};

} // namespace sgf
