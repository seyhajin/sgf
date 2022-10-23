#include "renderer.h"

namespace sgf {

Vector<RenderPassType> Renderer::renderPasses() const {
	return {RenderPassType::opaque};
}

void Renderer::attach(Scene* scene) {
	assert(scene && !m_scene);
	m_scene = scene;
}

void Renderer::detach(Scene* scene) {
	assert(scene && scene == m_scene);
	m_scene = nullptr;
}

} // namespace sgf
