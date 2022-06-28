#pragma once

#include "image.h"

namespace sgf {

Image* createImage(uint width, uint height, ImageFormat format);
Image* copyImage(Image* image);
Image* resizeImage(Image* image, uint width, uint height);

} // namespace sgf
