#pragma once

#include "renderparams.h"

#include <core3d/core3d.hh>

namespace sgf {

class RenderContext : public Object {
public:
	SGF_OBJECT_TYPE(RenderContext, Object);

	void beginScene(GraphicsContext* graphicsContext, const RenderParams* renderparams, uint width, uint height);

	Texture* endScene();

	GraphicsContext* graphicsContext() const {
		return m_graphicsContext;
	}

	const RenderParams* renderParams() const {
		return m_renderParams;
	}

	FrameBuffer* renderBuffer() const {
		return m_renderBuffer;
	}

	FrameBuffer* frameBuffer() const {
		return m_frameBuffer;
	}

	void swapFrameBuffers();

private:
	const RenderParams* m_renderParams = nullptr;
	SharedPtr<GraphicsContext> m_graphicsContext;
	SharedPtr<FrameBuffer> m_renderBuffer;
	SharedPtr<FrameBuffer> m_frameBuffer;
	SharedPtr<FrameBuffer> m_tmpBuffers[2];
};
using CRenderContext = const RenderContext&;

} // namespace sgf
