#include "modelinstance.h"

#include "modelrenderer.h"

#include "scene.h"

namespace sgf {

ModelInstance::ModelInstance(Scene* scene) : Entity(scene) {
	model.valueChanged.connect(this, [this](Model* newModel) {
		bool wasEnabled = enabled();
		disable();
		m_renderer = newModel ? newModel->getRenderer(this->scene()) : nullptr;
		if (wasEnabled) enable();
	});
}

void ModelInstance::onEnable() {
	if (!m_renderer) return;
	m_renderer->addInstance(this);
	scene()->addEntity(this);
}

void ModelInstance::onDisable() {
	if (!m_renderer) return;
	m_renderer->removeInstance(this);
	scene()->removeEntity(this);
}

} // namespace sgf
