#pragma once

#include "types.h"

#include <opengl/opengl.hh>

namespace sgf {

struct GLUniform {
	String name;
	uint id;
	GLint glSize;
	GLenum glType;
	GLint glLocation;
};

struct GLPixelFormat {
	GLint internalFormat;
	GLenum format;
	GLenum type;
};

constexpr GLPixelFormat glPixelFormats[] = {{GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE},
											{GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE},
											{GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE},
											{GL_RGB, GL_RGB, GL_UNSIGNED_BYTE},
											{GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE},
											{GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE},
											{GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE},
											{GL_RGBA16,GL_RGBA,GL_UNSIGNED_SHORT},
											{GL_R32F, GL_RED, GL_FLOAT},
											{GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT},
											{GL_RGBA16F, GL_RGBA, GL_FLOAT}};


//AttribFormat { none, float1, float2, float3, float4, sbyte4, ubyte4, ubyte2 };

constexpr GLint glAttribSizes[] = {0,1,2,3,4,4,4,2};

constexpr GLenum glAttribTypes[] = {GL_NONE,	GL_FLOAT, GL_FLOAT,			GL_FLOAT,
						  GL_FLOAT, GL_BYTE,  GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE};

void bindGLUniform(GLenum glType, GLint glLocation, CAny value);

void glAssert();

} // namespace sgf
