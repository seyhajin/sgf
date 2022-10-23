#pragma once

#include "renderpass.h"

namespace sgf {

class Scene;

class Renderer : public Object {
public:
	SGF_OBJECT_TYPE(Renderer, Object);

	virtual Vector<RenderPassType> renderPasses() const;

	Scene* scene() const {
		return m_scene;
	}

protected:
	virtual void onUpdate() {
	}

	virtual void onRender(RenderContext& rc, RenderPassType pass) = 0;

private:
	// ***** Internal *****
	friend class RenderPass;
	friend class Scene;

	Scene* m_scene = nullptr;

	void attach(Scene* scene);
	void detach(Scene* scene);
};

} // namespace sgf
