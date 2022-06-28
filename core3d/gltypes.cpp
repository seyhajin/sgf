#include "gltypes.h"

#include <stb/stb.hh>

#define USE_FILTER STBIR_FILTER_TRIANGLE

#define GL_FLOAT_PTR(P) reinterpret_cast<const GLfloat*>(P)

namespace sgf {

void setGLTextureData(uint width, uint height, TextureFormat format, TextureFlags flags, const void* data) {

	GLenum glTarget = GL_TEXTURE_2D;

	auto& glFormat = glPixelFormats[int(format)];

	auto imgData = static_cast<const uchar*>(data);
	uchar* oldData = nullptr;

	if (imgData && glFormat.internalFormat == GL_SRGB8_ALPHA8) {

		imgData = oldData = static_cast<uchar*>(malloc(width * 4 * height));

		for (uint y = 0; y < height; ++y) {
			auto dst = oldData + y * width * 4;
			const uchar* src = (uchar*)data + y * width * 4;
			for (uint x = 0; x < width; src += 4, dst += 4, ++x) {
				// ok, we want to premultiply rgb * alpha in linear space, but store srgb.
				// we can't just convert to linear and leave it as this can produce values > 255
				float a = src[3] / 255.0f;
				float r = std::pow(src[0] / 255.0f, 2.2f) * a;
				float g = std::pow(src[1] / 255.0f, 2.2f) * a;
				float b = std::pow(src[2] / 255.0f, 2.2f) * a;
				dst[0] = std::pow(r, 1 / 2.2f) * 255;
				dst[1] = std::pow(g, 1 / 2.2f) * 255;
				dst[2] = std::pow(b, 1 / 2.2f) * 255;
				dst[3] = src[3];
			}
		}
	}

	int w = int(width);
	int h = int(height);
	for (int mipLevel = 0;; ++mipLevel) {

		glTexImage2D(glTarget, mipLevel, glFormat.internalFormat, w, h, 0, glFormat.format, glFormat.type, imgData);

		if ((flags & TextureFlags::mipmap) != TextureFlags::mipmap) break;

		if (w == 1 && h == 1) break;

		int pw = w;
		int ph = h;
		w = w > 1 ? w / 2 : w;
		h = h > 1 ? h / 2 : h;

		if (!imgData) continue;

		if (bytesPerChannel(format) != 1) {
			glGenerateMipmap(glTarget);
			break;
		}

		auto newData = static_cast<uchar*>(malloc(w * bytesPerPixel(format) * h));

		if (glFormat.internalFormat == GL_SRGB8_ALPHA8) {

			stbir_resize_uint8_generic(imgData, pw, ph, 0, newData, w, h, 0, 4, 3, STBIR_FLAG_ALPHA_PREMULTIPLIED,
									   STBIR_EDGE_CLAMP, USE_FILTER, STBIR_COLORSPACE_SRGB, nullptr);

		} else if (glFormat.internalFormat == GL_SRGB8) {

			stbir_resize_uint8_generic(imgData, pw, ph, 0, newData, w, h, 0, 3, -1, STBIR_FLAG_ALPHA_PREMULTIPLIED,
									   STBIR_EDGE_CLAMP, USE_FILTER, STBIR_COLORSPACE_SRGB, nullptr);
		} else {
			stbir_resize_uint8_generic(imgData, pw, ph, 0, newData, w, h, 0, channelsPerPixel(format), -1,
									   STBIR_FLAG_ALPHA_PREMULTIPLIED, STBIR_EDGE_CLAMP, USE_FILTER,
									   STBIR_COLORSPACE_SRGB, nullptr);
		}

		if (oldData) free(oldData);
		imgData = oldData = newData;
	}
	if (oldData) free(oldData);
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

void bindGLUniform(GLenum glType, GLint glLocation, CAny value) {

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

} // namespace sgf
