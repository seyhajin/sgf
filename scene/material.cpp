#include "material.h"

#include "textureloader.h"

namespace sgf {

namespace {

Map<Vec4f, SharedPtr<Material>> g_matteCache;

}

Material::Material(CVec4f baseColor) {

	auto paramChanged = [this](const auto& param) { m_dirtyParams = true; };

	baseColorTexture = TextureLoader(Vec4f(1));
	emissiveTexture = TextureLoader(Vec4f(0));
	baseColorFactor = baseColor;
	emissiveFactor = 1;

	baseColorTexture.valueChanged.connect(paramChanged);
	metallicRoughnessTexture.valueChanged.connect(paramChanged);
	emissiveTexture.valueChanged.connect(paramChanged);
	occlusionTexture.valueChanged.connect(paramChanged);
	normalTexture.valueChanged.connect(paramChanged);

	baseColorFactor.valueChanged.connect(paramChanged);
	emissiveFactor.valueChanged.connect(paramChanged);

	m_uniformBuffer = graphicsDevice()->createGraphicsBuffer(BufferType::uniform, sizeof(m_materialParams), nullptr);
}

void Material::bind(GraphicsContext* gc) {

	gc->setTexture("baseColorTexture", baseColorTexture.value().open());
	gc->setTexture("emissiveTexture", emissiveTexture.value().open());
#if 0
	gc->setTexture("metallicRoughnessTexture", metallicRoughnessTexture.value().open());
	gc->setTexture("occlusionTexture", occlusionTexture.value().open());
	gc->setTexture("normalTexture", normalTexture.value().open());
#endif

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

Material* matteMaterial(CVec4f color) {

	auto it = g_matteCache.find(color);
	if (it != g_matteCache.end()) return it->second;

	auto material = new Material(color);
	g_matteCache.insert(std::make_pair(color, material));

	return material;
}

Material* errorMaterial() {
	return matteMaterial(Vec4f(1, 0, 1, 1));
}

} // namespace sgf
