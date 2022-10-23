#include "gltypes.h"

#include <stb/stb.hh>

#define USE_FILTER STBIR_FILTER_TRIANGLE

#define GL_FLOAT_PTR(P) reinterpret_cast<const GLfloat*>(P)

namespace sgf {

void bindGLUniform(GLenum glType, GLint glLocation, CAny value) {

	assert(value.exists());

	switch (glType) {
	case GL_FLOAT:
		glUniform1fv(glLocation, 1, GL_FLOAT_PTR(&value.get<float>()));
		break;
	case GL_FLOAT_VEC2:
		glUniform2fv(glLocation, 1, GL_FLOAT_PTR(&value.get<Vec2f>()));
		break;
	case GL_FLOAT_VEC3:
		glUniform3fv(glLocation, 1, GL_FLOAT_PTR(&value.get<Vec3f>()));
		break;
	case GL_FLOAT_VEC4:
		glUniform4fv(glLocation, 1, GL_FLOAT_PTR(&value.get<Vec4f>()));
		break;
	case GL_FLOAT_MAT3:
		glUniformMatrix3fv(glLocation, 1, false, GL_FLOAT_PTR(&value.get<Mat3f>()));
		break;
	case GL_FLOAT_MAT4:
		glUniformMatrix4fv(glLocation, 1, false, GL_FLOAT_PTR(&value.get<Mat4f>()));
		break;
	default:
		panic("OOPS");
	}
}

void glAssert() {
	auto err = glGetError();
	if (!err) return;
	const char* msg = "?????";
	switch (err) {
	case GL_INVALID_ENUM:
		msg = "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		msg = "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		msg = "GL_INVALID_OPERATION";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		msg = "GL_INVALID_FRAMEBUFFER_OPERATION";
		break;
	case GL_OUT_OF_MEMORY:
		msg = "GL_OUT_OF_MEMORY";
		break;
#ifndef USE_OPENGLES
	case GL_STACK_UNDERFLOW:
		msg = "GL_STACK_UNDERFLOW";
		break;
	case GL_STACK_OVERFLOW:
		msg = "GL_STACK_OVERFLOW";
		break;
#endif
	}
	panic(msg);
}

} // namespace sgf
