#include "imageutils.h"
#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
//#define STBIR_DEFAULT_FILTER_DOWNSAMPLE STBIR_FILTER_BOX
//#define STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_BOX
//#define STBIR_DEFAULT_FILTER_DOWNSAMPLE STBIR_FILTER_TRIANGLE
//#define STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_TRIANGLE
#define STBIR_DEFAULT_FILTER_DOWNSAMPLE STBIR_FILTER_CATMULLROM
#define STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_CATMULLROM
//#define STBIR_DEFAULT_FILTER_DOWNSAMPLE STBIR_FILTER_CUBICBSPLINE
//#define STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_CUBICBSPLINE
#include <stb/stb_image_resize.h>

namespace sgf {

namespace {

StringMap<Image*> g_imageCache;
Map<Image*, String> g_imagePaths;

} // namespace

Image* createImage(uint width, uint height, ImageFormat format) {

	uint pitch = width * bytesPerPixel(format);
	void* data = malloc(pitch * height);

	auto image = new Image(data, width, height, pitch, format);
	image->deleted.connect([data] { free(data); });

	return image;
}

Image* copyImage(Image* image) {

	auto newImage = createImage(image->width, image->height, image->format);

	uint rowSize = image->width * bytesPerPixel(image->format);

	for (uint y = 0; y < image->height; ++y) {
		memcpy(newImage->data + newImage->pitch * y, image->data + image->pitch * y, rowSize);
	}

	return newImage;
}

Image* resizeImage(Image* image, uint width, uint height) {

	uint channels = 0;

	switch (image->format) {
	case ImageFormat::alpha8:
	case ImageFormat::intensity8:
	case ImageFormat::r32f:
		channels = 1;
		break;
	case ImageFormat::intensityAlpha16:
		channels = 2;
		break;
	case ImageFormat::rgb24:
		channels = 3;
		break;
	case ImageFormat::rgba32:
		channels = 4;
		break;
	}

	Image* newImage = createImage(width, height, image->format);

	if (image->format == ImageFormat::r32f) {
		stbir_resize_float((float*)image->data, image->width, image->height, image->pitch, (float*)newImage->data,
						   newImage->width, newImage->height, newImage->pitch, channels);
	} else {
		stbir_resize_uint8(image->data, image->width, image->height, image->pitch, newImage->data, newImage->width,
						   newImage->height, newImage->pitch, channels);
	}

	return newImage;
}

} // namespace sgf
