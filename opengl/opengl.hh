#pragma once

#ifdef USE_OPENGLES
#include <GLES3/gl31.h>
#else
#include <GL/glew.h>
#endif

namespace sgf {
//  Needs to be a current opengl context when this is called
void initOpenGL();
} // namespace sgf
