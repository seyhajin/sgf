#pragma once

#include <core/core.hh>
#include <geom/geom.hh>

namespace sgf {

enum struct ImageFormat { alpha8, intensity8, intensityAlpha16, rgb24, rgba32, r32f };

inline uint bytesPerPixel(ImageFormat format) {
	uint r[]{1, 1, 2, 3, 4, 4};
	return r[int(format)];
}

class Image : public Shared {
public:
	uint8_t* const data;
	uint const width;
	uint const height;
	uint const pitch;
	ImageFormat const format;

	Image(void* data, uint width, uint height, uint pitch, ImageFormat format);

	void setPixel(int x, int y, CVec4f pixel) const;

	Vec4f getPixel(int x, int y) const;
	Vec4f getPixel(float x, float y) const;
};

} // namespace sgf
