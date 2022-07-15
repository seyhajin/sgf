#pragma once

#include <core/core.hh>

#ifdef USE_OPENGLES
#include <GLES3/gl31.h>

// TODO: Sort out GLES extensions issues...
#define GL_RGBA16 0x805B
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

#else
#include <GL/glew.h>
#endif

namespace sgf {
//  Needs to be a current opengl context when this is called
void initGLContext();

bool checkGLExtension(CString ext);

} // namespace sgf
