#pragma once

#include <core3d/core3d.hh>

namespace sgf {

Texture* createTexture(uint rgba);
Texture* createTexture(CVec4f color);
Texture* loadTexture(CString path, TextureFormat format, TextureFlags flags);

struct TextureLoader {
	TextureLoader(Texture* texture) {
		assert(texture);
		m_texture=texture;
	}

	TextureLoader(CVec4f color = {1, 0, 1, 1}) {
		m_loadFunc = [color] { return createTexture(color); };
	};

	TextureLoader(CString assetPath, TextureFormat format = TextureFormat::srgba32, TextureFlags flags = TextureFlags::mipmap) {
		m_loadFunc = [assetPath, format, flags] { return loadTexture(resolveAssetPath(assetPath), format, flags); };
	}

	Texture* open() const {
		if (!m_texture) m_texture = m_loadFunc();
		return m_texture;
	}

	bool operator==(const TextureLoader& that) const {
		return m_loadFunc == that.m_loadFunc;
	}

	bool operator!=(const TextureLoader& that) const {
		return m_loadFunc != that.m_loadFunc;
	}

	bool operator<(const TextureLoader& that) const {
		return m_loadFunc < that.m_loadFunc;
	}

private:
	using LoadFunc = Function<Texture*()>;

	mutable SharedPtr<Texture> m_texture;
	LoadFunc m_loadFunc;
};

} // namespace sgf
