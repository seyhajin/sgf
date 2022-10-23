#pragma once

#include "renderer.h"
#include "textureasset.h"

namespace sgf {

class EnvRenderer : public Renderer {
public:
	Property<TextureAsset> skyTexture;
	Property<Vec4f> fogColor;
	Property<float> fogRange;
	Property<float> fogPower;

	EnvRenderer();

private:
	SharedPtr<GraphicsBuffer> m_paramsBuffer;

	bool m_dirtyParams=true;

	Vector<RenderPassType> renderPasses() const override;

	void onRender(RenderContext& rc, RenderPassType pass) override;
};

} // namespace wb
