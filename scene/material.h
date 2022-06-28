#pragma once

#include "textureloader.h"

#include "renderparams.h"

namespace sgf {

class Material : public Shared {
public:
	Property<TextureLoader> baseColorTexture;
	Property<TextureLoader> metallicRoughnessTexture;
	Property<TextureLoader> emissiveTexture;
	Property<TextureLoader> occlusionTexture;
	Property<TextureLoader> normalTexture;

	Property<Vec4f> baseColorFactor;
	Property<Vec3f> emissiveFactor;

	Property<BlendMode> blendMode;
	Property<CullMode> cullMode;

	Material(CVec4f baseColor);

	void bind(GraphicsContext* gc);

private:
	mutable bool m_dirtyParams = true;

	MaterialParams m_materialParams;
	SharedPtr<UniformBuffer> m_uniformBuffer;
};

} // namespace sgf
