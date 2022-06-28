#include "renderpass.h"

#include "renderer.h"

namespace sgf {

RenderPass::RenderPass(RenderPassType type, DepthMode depthMode, BlendMode blendMode, CullMode cullMode)
	: type(type), depthMode(depthMode), blendMode(blendMode), cullMode(cullMode) {
}

void RenderPass::clear() {
	m_renderers.clear();
}

void RenderPass::addRenderer(Renderer* r) {
	m_renderers.push_back(r);
}

void RenderPass::removeRenderer(Renderer* r) {
	remove(m_renderers, r);
}

void RenderPass::update() {
	for (Renderer* r : m_renderers) r->onUpdate();
}

void RenderPass::render(RenderContext& rc) {

	auto gc = rc.graphicsContext();

	for (Renderer* renderer : m_renderers) {

		gc->setDepthMode(depthMode);
		gc->setBlendMode(blendMode);
		gc->setCullMode(cullMode);

		renderer->onRender(rc, type);
	}
}

} // namespace sgf
