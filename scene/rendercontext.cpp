#include "rendercontext.h"

namespace sgf {

void RenderContext::beginScene(GraphicsContext* graphicsContext, const RenderParams* renderParams, uint width,
							   uint height) {

	m_graphicsContext = graphicsContext;
	m_renderParams = renderParams;

	if (!m_renderBuffer || m_renderBuffer->width != width || m_renderBuffer->height != height) {

		m_renderBuffer = nullptr;
		m_tmpBuffers[0] = nullptr;
		m_tmpBuffers[1] = nullptr;
		m_frameBuffer = nullptr;

		auto colorTexture = graphicsDevice()->createTexture(width, height, TextureFormat::rgba64f,
															TextureFlags::linear | TextureFlags::clampST, nullptr);

		auto depthTexture =
			graphicsDevice()->createTexture(width, height, TextureFormat::depth32f, TextureFlags::clampST, nullptr);

		m_renderBuffer = graphicsDevice()->createFrameBuffer(colorTexture, depthTexture);

	} else {

		if (m_tmpBuffers[1] && m_renderBuffer->colorTexture.value() == m_tmpBuffers[1]->colorTexture.value()) {
			std::swap(m_tmpBuffers[0], m_tmpBuffers[1]);
		}
	}

	m_frameBuffer = m_renderBuffer;

	m_graphicsContext->setFrameBuffer(m_frameBuffer);
}

Texture* RenderContext::endScene() {
	auto texture = m_frameBuffer->colorTexture.value();
	m_frameBuffer = nullptr;
	m_renderParams = nullptr;
	m_graphicsContext = nullptr;
	return texture;
}

void RenderContext::swapFrameBuffers() {

	if (!m_tmpBuffers[0]) {
		m_tmpBuffers[0] = graphicsDevice()->createFrameBuffer(m_renderBuffer->colorTexture, nullptr);
		auto colorTexture =
			graphicsDevice()->createTexture(m_renderBuffer->width, m_renderBuffer->height, TextureFormat::rgba64f,
											TextureFlags::linear | TextureFlags::clampST, nullptr);

		m_tmpBuffers[1] = graphicsDevice()->createFrameBuffer(colorTexture, nullptr);
	}

	std::swap(m_tmpBuffers[0], m_tmpBuffers[1]);

	m_frameBuffer = m_tmpBuffers[0];

	m_graphicsContext->setFrameBuffer(m_frameBuffer);
}

} // namespace sgf
