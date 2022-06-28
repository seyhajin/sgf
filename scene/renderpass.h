#pragma once

#include "rendercontext.h"

namespace sgf {

class Renderer;

enum struct RenderPassType { background, opaque, blended, shadows, sprites, postfx, overlay, end };

static constexpr uint numRenderPassTypes = uint(RenderPassType::end);

class RenderPass : public Object {
public:
	SGF_OBJECT_TYPE(RenderPass, Object);

	RenderPassType const type;
	DepthMode const depthMode;
	BlendMode const blendMode;
	CullMode const cullMode;

	RenderPass(RenderPassType type, DepthMode depthMode, BlendMode blendMode, CullMode cullMode);

	void clear();

	CVector<Renderer*> renderers() const {
		return m_renderers;
	}

	void addRenderer(Renderer* renderer);
	void removeRenderer(Renderer* renderer);

	void update();
	void render(RenderContext& rc);

private:
	friend class Scene;

	Vector<Renderer*> m_renderers;
};
using CRenderPass = const RenderPass&;

} // namespace sgf
