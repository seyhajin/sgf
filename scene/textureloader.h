#pragma once

#include <core3d/core3d.hh>
#include <image/image.hh>

namespace sgf {

TextureFormat textureFormat(ImageFormat imageFormat);
ImageFormat imageFormat(TextureFormat textureFormat);

Texture* createTexture(uint rgba);
Texture* createTexture(CVec4f color);
Texture* loadTexture(CString assetPath, TextureFormat format = TextureFormat::srgba32,
					 TextureFlags flags = TextureFlags::none);

String assetPath(Texture* texture);

}
