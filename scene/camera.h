#pragma once

#include "entity.h"

namespace sgf {

class Camera : public Entity {
public:
	SGF_OBJECT_TYPE(Camera, Entity);

	Property<float> fovY{45.0f};
	Property<float> zNear{.1f};
	Property<float> zFar{1000.0f};
	Property<Recti> viewport{{0, 0, 640, 480}};

	explicit Camera(Entity* parent = nullptr);

	Mat4f projectionMatrix() const;

private:
	mutable bool m_projDirty = true;
	mutable Mat4f m_projMatrix;

	void onSetEnabled(bool enabled) override;
};

} // namespace sgf
