#pragma once

#include "textureasset.h"

#include "renderparams.h"

namespace sgf {

class Material : public Shared {
public:
	Property<TextureAsset> baseColorTexture;
	Property<TextureAsset> metallicRoughnessTexture;
	Property<TextureAsset> emissiveTexture;
	Property<TextureAsset> occlusionTexture;
	Property<TextureAsset> normalTexture;

	Property<Vec4f> baseColorFactor;
	Property<Vec3f> emissiveFactor;

	Property<BlendMode> blendMode;
	Property<CullMode> cullMode;

	Property<bool> flatShaded;

	Material(CVec4f baseColor={1});

	void bindShaderParams(GraphicsContext* gc);

private:
	mutable bool m_dirtyParams = true;

	MaterialParams m_materialParams;
	SharedPtr<GraphicsBuffer> m_uniformBuffer;
};

Material* createMatteMaterial(CVec4f color);
Material* loadMatteMaterial(CString colorTexture,TextureFormat format = TextureFormat::srgba32, TextureFlags flags = TextureFlags::mipmap);
Material* defaultMaterial();
Material* errorMaterial();

} // namespace sgf
