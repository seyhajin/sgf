#include "image.h"

namespace sgf {

Image::Image(void* data, uint width, uint height, uint pitch, ImageFormat format)
	: data((uchar*)data), width(width), height(height), pitch(pitch), format(format) {
	assert(data);
}

Vec4f Image::getPixel(int x, int y) const {

	if (uint(x) >= width || uint(y) >= height) return {};

	static constexpr float sc = 1.0f / 255.0f;

	uchar* p = data + y * pitch + x * bytesPerPixel(format);
	switch (format) {
	case ImageFormat::alpha8:
		return {0, 0, 0, p[0] * sc};
	case ImageFormat::intensity8:
		return {p[0] * sc, p[0] * sc, p[0] * sc, 1};
	case ImageFormat::intensityAlpha16:
		return {p[0] * sc, p[0] * sc, p[0] * sc, p[1] * sc};
	case ImageFormat::rgb24:
		return {p[0] * sc, p[1] * sc, p[2] * sc, 1};
	case ImageFormat::rgba32:
		return {p[0] * sc, p[1] * sc, p[2] * sc, p[3] * sc};
	case ImageFormat::r32f:
		return {*(float*)p, 0, 0, 1};
	}
	return {};
}

Vec4f Image::getPixel(float x, float y) const {
	float fx = std::floor(x);
	float fy = std::floor(y);
	int ix = int(fx);
	int iy = int(fy);
	auto x0 = getPixel(ix, iy);
	auto x1 = getPixel(ix + 1, iy);
	auto y0 = (x1 - x0) * (x - fx) + x0;
	auto x2 = getPixel(ix, iy + 1);
	auto x3 = getPixel(ix + 1, iy + 1);
	auto y1 = (x3 - x2) * (x - fx) + x2;
	return (y1 - y0) * (y - fy) + y0;
}

void Image::setPixel(int x, int y, CVec4f pixel) const {

	if (uint(x) >= width || uint(y) >= height) return;

	uchar* p = data + y * pitch + x * bytesPerPixel(format);
	switch (format) {
	case ImageFormat::alpha8:
		p[0] = std::round(pixel.w * 255.0f);
		return;
	case ImageFormat::intensity8:
		p[0] = std::round(pixel.x * 255.0f);
		return;
	case ImageFormat::intensityAlpha16:
		p[0] = std::round(pixel.x * 255.0f);
		p[1] = std::round(pixel.w * 255.0f);
		return;
	case ImageFormat::rgb24:
		p[0] = std::round(pixel.x * 255.0f);
		p[1] = std::round(pixel.y * 255.0f);
		p[2] = std::round(pixel.z * 255.0f);
		return;
	case ImageFormat::rgba32:
		p[0] = std::round(pixel.x * 255.0f);
		p[1] = std::round(pixel.y * 255.0f);
		p[2] = std::round(pixel.z * 255.0f);
		p[3] = std::round(pixel.w * 255.0f);
		return;
	case ImageFormat::r32f:
		*(float*)p = pixel.x;
		return;
	}
}

} // namespace sgf
