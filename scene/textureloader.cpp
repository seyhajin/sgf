#include "textureloader.h"

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

TextureFormat textureFormat(ImageFormat imgFmt) {
	switch (imgFmt) {
	case ImageFormat::alpha8:
		return TextureFormat::alpha8;
	case ImageFormat::intensity8:
		return TextureFormat::intensity8;
	case ImageFormat::intensityAlpha16:
		return TextureFormat::intensityAlpha16;
	case ImageFormat::rgb24:
		return TextureFormat::rgb24;
	case ImageFormat::rgba32:
		return TextureFormat::rgba32;
	case ImageFormat::r32f:
		return TextureFormat::r32f;
	default:
		panic("OOPS");
	}
	return {};
}

ImageFormat imageFormat(TextureFormat texFmt) {
	switch (texFmt) {
	case TextureFormat::alpha8:
		return ImageFormat::alpha8;
	case TextureFormat::intensity8:
		return ImageFormat::intensity8;
	case TextureFormat::intensityAlpha16:
		return ImageFormat::intensityAlpha16;
	case TextureFormat::rgb24:
	case TextureFormat::srgb24:
		return ImageFormat::rgb24;
	case TextureFormat::rgba32:
	case TextureFormat::srgba32:
		return ImageFormat::rgba32;
	case TextureFormat::r32f:
		return ImageFormat::r32f;
	default:
		panic("OOPS");
	};
	return {};
}

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

	auto imgfmt = imageFormat(texfmt);

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
