#pragma once

#include <core3d/core3d.hh>

namespace sgf {

Texture* createTexture(uint rgba);
Texture* createTexture(CVec4f color);
Texture* loadTexture(CString path, TextureFormat format, TextureFlags flags);

struct TextureLoader;
using CTextureLoader = const TextureLoader&;

struct TextureLoader {

	TextureLoader() {
		m_loader = []()->Texture*{return nullptr;};
	}

	TextureLoader(CTextureLoader that) {
		m_texture = that.m_texture;
		m_loader = that.m_loader;
	}

	TextureLoader(TextureLoader&& that) noexcept{
		m_texture = std::move(that.m_texture);
		m_loader = std::move(that.m_loader);
	}

	TextureLoader(CVec4f color) {
		m_loader = [color] { return createTexture(color); };
	};

	TextureLoader(Texture* texture) {
		assert(texture);
		m_texture = texture;
	}
	
	TextureLoader(CString path, TextureFormat format = TextureFormat::srgba32,
				  TextureFlags flags = TextureFlags::mipmap) {
		m_loader = [path, format, flags] { return loadTexture(resolveAssetPath(path), format, flags); };
	}

	TextureLoader& operator=(CTextureLoader that) {
		m_texture = that.m_texture;
		m_loader = that.m_loader;
		return *this;
	}

	TextureLoader& operator=(TextureLoader&& that) noexcept {
		if (*this == that) return *this;
		m_texture = std::move(that.m_texture);
		m_loader = std::move(that.m_loader);
		return *this;
	}

	bool operator==(CTextureLoader that) const {
		return m_loader == that.m_loader;
	}

	bool operator!=(CTextureLoader that) const {
		return m_loader != that.m_loader;
	}

	bool operator<(CTextureLoader that) const {
		return m_loader < that.m_loader;
	}

	Texture* open() const {
		if (!m_texture) m_texture = m_loader();
		return m_texture;
	}

private:
	using loader = Function<Texture*()>;

	mutable SharedPtr<Texture> m_texture;
	loader m_loader;
};

} // namespace sgf
