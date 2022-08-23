#include "model.h"

#include "scene.h"

namespace sgf {

Model::Model(Scene* scene) : Entity(scene) {
	renderData.valueChanged.connect(this, [this](ModelRenderData* newData) {
		bool wasEnabled = enabled();
		disable();
		m_renderer = newData ? newData->getRenderer(this->scene()) : nullptr;
		if (wasEnabled) enable();
	});
}

void Model::onEnable() {
	if (!m_renderer) return;
	m_renderer->addInstance(this);
	scene()->addEntity(this);
}

void Model::onDisable() {
	if (!m_renderer) return;
	m_renderer->removeInstance(this);
	scene()->removeEntity(this);
}

} // namespace sgf
