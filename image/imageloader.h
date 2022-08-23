#pragma once

#include "image.h"

namespace sgf {

Image* loadImage(CString assetPath);

Image* loadImage(CString assetPath, ImageFormat format);

} // namespace sgf
