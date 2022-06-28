#include "material.h"

#include "textureloader.h"

namespace sgf {

Material::Material(CVec4f baseColor) {

	auto paramChanged = [this](const auto& param) { m_dirtyParams = true; };

	baseColorTexture = TextureLoader(Vec4f(1));
	emissiveTexture = TextureLoader(Vec4f(1));
	baseColorFactor = baseColor;
	emissiveFactor = 0;

	baseColorTexture.valueChanged.connect(paramChanged);
	metallicRoughnessTexture.valueChanged.connect(paramChanged);
	emissiveTexture.valueChanged.connect(paramChanged);
	occlusionTexture.valueChanged.connect(paramChanged);
	normalTexture.valueChanged.connect(paramChanged);

	baseColorFactor.valueChanged.connect(paramChanged);
	emissiveFactor.valueChanged.connect(paramChanged);

	m_uniformBuffer = graphicsDevice()->createUniformBuffer(sizeof(m_materialParams), nullptr);
}

void Material::bind(GraphicsContext* gc) {

	gc->setTextureUniform("baseColorTexture", baseColorTexture.value().open());
	gc->setTextureUniform("metallicRoughnessTexture", metallicRoughnessTexture.value().open());
	gc->setTextureUniform("emissiveTexture", emissiveTexture.value().open());
	gc->setTextureUniform("occlusionTexture", occlusionTexture.value().open());
	gc->setTextureUniform("normalTexture", normalTexture.value().open());

	if (m_dirtyParams) {
		m_materialParams.baseColorFactor = baseColorFactor;
		m_materialParams.emissiveFactor = emissiveFactor;
		m_uniformBuffer->updateData(0, sizeof(m_materialParams), &m_materialParams);
		m_dirtyParams = false;
	}
	gc->setUniformBuffer("materialParams", m_uniformBuffer);

	gc->setBlendMode(blendMode);
	gc->setCullMode(cullMode);
}

} // namespace sgf
