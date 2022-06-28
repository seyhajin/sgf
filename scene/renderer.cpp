#include "renderer.h"

namespace sgf {

void Renderer::attach(Scene* scene) {
	assert(scene && !m_scene);
	m_scene = scene;
}

void Renderer::detach(Scene* scene) {
	assert(scene && scene == m_scene);
	m_scene = nullptr;
}

} // namespace sgf
