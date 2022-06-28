#include "textureloader.h"

#include <image/image.hh>

#include <core3d/core3d.hh>

namespace sgf {

namespace {

Map<uint, SharedPtr<Texture>> g_cache;

}

Texture* createTexure(uint rgba) {

	auto it = g_cache.find(rgba);
	if (it != g_cache.end()) return it->second;

	auto texture = graphicsDevice()->createTexture(1, 1, TextureFormat::rgba32, TextureFlags::none, &rgba);

	g_cache.insert(std::make_pair(rgba, texture));

	return texture;
}

Texture* createTexure(CVec4f color) {
	uint rgba = (uint(color.w * 255.0f) << 24) | (uint(color.z * 255.0f) << 16) | (uint(color.y * 255.0f) << 8) |
				uint(color.x * 255.0f);
	return createTexture(rgba);
}

Texture* loadTexture(CString path, TextureFormat texfmt, TextureFlags flags) {

	ImageFormat imgfmt;
	switch (texfmt) {
	case TextureFormat::alpha8:
		imgfmt = ImageFormat::alpha8;
		break;
	case TextureFormat::intensity8:
		imgfmt = ImageFormat::intensity8;
		break;
	case TextureFormat::intensityAlpha16:
		imgfmt = ImageFormat::intensityAlpha16;
		break;
	case TextureFormat::rgb24:
	case TextureFormat::srgb24:
		imgfmt = ImageFormat::rgb24;
		break;
	case TextureFormat::rgba32:
	case TextureFormat::srgba32:
		imgfmt = ImageFormat::rgba32;
		break;
	case TextureFormat::r32f:
		imgfmt = ImageFormat::r32f;
		break;
	default:
		panic("OOPS");
	};

	auto image = loadImage(path, imgfmt);
	auto texture = graphicsDevice()->createTexture(image->width, image->height, texfmt, flags, image->data);

	return texture;
}

} // namespace sgf
