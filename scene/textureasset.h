#pragma once

#include "textureloader.h"

namespace sgf {

class TextureAsset;
using CTextureAsset = const TextureAsset&;

class TextureAsset {
public:
	TextureAsset() : m_loader([]() -> Texture* { return nullptr; }) {
	}

	TextureAsset(Texture* texture) : m_texture(texture) {
		assert(texture);
	}

	TextureAsset(uint rgba) : m_loader([rgba] { return createTexture(rgba); }) {
	}

	TextureAsset(CVec4f color) : m_loader([color] { return createTexture(color); }) {
	}

	TextureAsset(CString assetPath, TextureFormat format = TextureFormat::srgba32,
				 TextureFlags flags = TextureFlags::mipmap)
		: m_loader([assetPath, format, flags] { return loadTexture(assetPath, format, flags); }) {
	}

	TextureAsset(CTextureAsset) = default;

	TextureAsset(TextureAsset&&) = default;

	TextureAsset& operator=(CTextureAsset that) = default;

	TextureAsset& operator=(TextureAsset&& that) = default;

	bool operator==(CTextureAsset that) const {
		return m_loader == that.m_loader;
	}

	bool operator!=(CTextureAsset that) const {
		return m_loader != that.m_loader;
	}

	bool operator<(CTextureAsset that) const {
		return m_loader < that.m_loader;
	}

	Texture* open() const {
		if (!m_texture) m_texture = m_loader();
		return m_texture;
	}

private:
	mutable SharedPtr<Texture> m_texture;
	Function<Texture*()> m_loader;
};

} // namespace sgf
