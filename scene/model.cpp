#include "model.h"

#include "scene.h"

namespace sgf {

Model::Model(ModelRenderData* data) : renderData(data) {
}

Model::Model(const Mesh* mesh) {
	renderData = createModelRenderData(mesh);
}

void Model::onEnable() {

	auto updateRenderer = [this](ModelRenderData* data) {
		if (m_renderer) {
			m_renderer->removeInstance(this);
			scene->removeEntity(this);
		}

		m_renderer = renderData.value() ? renderData.value()->getRenderer(this->scene) : nullptr;
		if(!m_renderer) return;

		m_renderer->addInstance(this);
		scene->addEntity(this);
	};

	renderData.valueChanged.connect(this, updateRenderer);

	updateRenderer(renderData.value());
}

void Model::onDisable() {

	renderData.valueChanged.disconnect(this);

	if (!m_renderer) return;

	m_renderer->removeInstance(this);
	m_renderer = nullptr;
	scene->removeEntity(this);
}

} // namespace sgf
