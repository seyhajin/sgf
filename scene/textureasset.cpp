#include "textureasset.h"

#include <image/image.hh>

#include <core3d/core3d.hh>

namespace sgf {

namespace {

// Maps RGBA values to textures.
Map<uint, SharedPtr<Texture>> g_ccache;

// Maps asset paths to textures.
// It's OK to not use a SharedPtr<Texture> here because texture is removed from the map immediately upon deletion.
StringMap<Texture*> g_fcache;

// Maps textures to asset paths.
Map<Texture*, String> g_rcache;

} // namespace

Texture* createTexture(uint rgba) {

	auto it = g_ccache.find(rgba);
	if (it != g_ccache.end()) return it->second;

	auto texture = graphicsDevice()->createTexture(1, 1, TextureFormat::rgba32, TextureFlags::none, &rgba);

	g_ccache.insert(std::make_pair(rgba, texture));

	return texture;
}

Texture* createTexture(CVec4f color) {
	uint rgba = (uint(color.w * 255.0f) << 24) | (uint(color.z * 255.0f) << 16) | (uint(color.y * 255.0f) << 8) |
				uint(color.x * 255.0f);
	return createTexture(rgba);
}

Texture* loadTexture(CString assetPath, TextureFormat texfmt, TextureFlags flags) {

	auto it = g_fcache.find(assetPath);
	if (it != g_fcache.end()) {
		auto texture = it->second;

		// TODO: Implement a multimap or samplers if we want same file/different format etc.
		if (texture->format != texfmt || texture->flags != flags) panic("OOPS");

		return texture;
	}

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

	auto image = loadImage(assetPath, imgfmt);
	auto texture = graphicsDevice()->createTexture(image->width, image->height, texfmt, flags, image->data);

	g_fcache.insert(std::make_pair(assetPath, texture));
	g_rcache.insert(std::make_pair(texture, assetPath));

	texture->deleted.connect([assetPath, texture] {
		g_rcache.erase(texture);
		g_fcache.erase(assetPath);
	});

	return texture;
}

String assetPath(Texture* texture) {
	auto it = g_rcache.find(texture);
	return it != g_rcache.end() ? it->second : String{};
}

} // namespace sgf
