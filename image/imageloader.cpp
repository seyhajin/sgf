#include "imageloader.h"

#include <stb/stb.hh>

namespace sgf {

namespace {

Image* loadImageR32f(CString path) {

	int width, height, n;

	auto srcData = static_cast<ushort*>(stbi_load_16(path.c_str(), &width, &height, &n, 1));
	if (!srcData) panic("Can't load image:" + path);

	auto data = static_cast<ushort*>(malloc(width * 4 * height));
	for (int y = 0; y < height; ++y) {
		const ushort* src = srcData + y * width;
		auto dst = data + y * width;
		for (int x = 0; x < width; ++x) { *dst++ = ushort(float(*src++) / 65536.0f); }
	}

	stbi_image_free(srcData);

	auto image = new Image(data, width, height, width * 4, ImageFormat::r32f);

	image->deleted.connect([data] { free(data); });

	return image;
}

} // namespace

Image* loadImage(CString assetPath) {

	auto path = resolveAssetPath(assetPath);

	int width, height, bpp;

	auto data = stbi_load(path.c_str(), &width, &height, &bpp, 0);
	if (!data) panic("Can't load image:" + path);

	assert(bpp > 0 && bpp < 4);

	ImageFormat formats[] = {
		{}, ImageFormat::alpha8, ImageFormat::intensityAlpha16, ImageFormat::rgb24, ImageFormat::rgba32};

	auto image = new Image(data, width, height, width * bpp, formats[bpp]);

	image->deleted.connect([data] { stbi_image_free(data); });

	return image;
}

Image* loadImage(CString assetPath, ImageFormat format) {

	auto path = resolveAssetPath(assetPath);

	if (format == ImageFormat::r32f) return loadImageR32f(path);

	uint bpp = bytesPerPixel(format);
	int width, height, n;

	auto data = stbi_load(path.c_str(), &width, &height, &n, bpp);
	if (!data) panic("Can't load image:" + path);

	auto image = new Image(data, width, height, width * bpp, format);

	image->deleted.connect([data] { stbi_image_free(data); });

	return image;
}

} // namespace sgf
