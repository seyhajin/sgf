#include "envrenderer.h"

#include "assets/shaders/envparams.h"

#include "shaderasset.h"

namespace sgf {

namespace {

ShaderAsset g_shader("shaders/envrenderer.glsl");

} // namespace

EnvRenderer::EnvRenderer() {
	m_paramsBuffer = graphicsDevice()->createGraphicsBuffer(BufferType::uniform, sizeof(EnvParams), nullptr);

	fogColor.valueChanged.connect(this, [this](CVec4f){m_dirtyParams=true;});
	fogRange.valueChanged.connect(this, [this](float){m_dirtyParams=true;});
	fogPower.valueChanged.connect(this, [this](float){m_dirtyParams=true;});

}

Vector<RenderPassType> EnvRenderer::renderPasses() const {
	return {RenderPassType::postfx};
}

void EnvRenderer::onRender(RenderContext& rc, RenderPassType pass) {
	auto gc = rc.graphicsContext();

	// TODO: Only do this when dirty
	if(m_dirtyParams) {
		EnvParams params{fogColor, fogRange, fogPower};
		m_paramsBuffer->updateData(0, sizeof(EnvParams), &params);
		m_dirtyParams=false;
	}

	gc->setShader(g_shader.open());

	gc->setTexture("envSkyTexture", skyTexture.value().open());
	gc->setUniformBuffer("envParams", m_paramsBuffer);

	gc->setTexture("sourceTexture", rc.frameBuffer()->colorTexture);
	gc->setTexture("depthTexture", rc.frameBuffer()->depthTexture);

	rc.swapFrameBuffers();

	gc->drawGeometry(3, 0, 6, 1);
}

} // namespace sgf
