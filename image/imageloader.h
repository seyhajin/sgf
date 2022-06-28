#pragma once

#include "image.h"

namespace sgf {

Image* loadImage(CString path);
Image* loadImage(CString path, ImageFormat format);

struct ImageLoader {

	ImageLoader(CString path) {
		m_loadFunc = [path] { return loadImage(path); };
	}

	ImageLoader(CString path, ImageFormat format) {
		m_loadFunc = [path, format] { return loadImage(path, format); };
	}

	Image* open() {
		if (!m_image) m_image = m_loadFunc();
		return m_image;
	}

private:
	using LoadFunc = Function<Image*()>;

	SharedPtr<Image> m_image;
	LoadFunc m_loadFunc;
};

} // namespace sgf
