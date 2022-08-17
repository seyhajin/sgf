#include "xrcamera.h"

#include <new>

#include <window/window.h>
#include <scene/scene.h>
#include <webxr/xrsystem.h>

namespace sgf {

void XRCamera::beginFrame(XRFrame* frame) {
	m_frame = frame;
}

void XRCamera::endFrame() {
	m_frame = nullptr;
}

Vector<CameraView> XRCamera::validateViews() const {

	if (!m_frame) {
		// Default to plain perspective camera if not inside an XRFrame...
		Recti viewport{0, scene()->graphicsDevice()->window->size()};

		float yh = std::tan(fovY * degreesToRadians) * zNear;
		float ar = float(viewport.width()) / float(viewport.height());
		auto projMatrix = Mat4f::frustum(-yh * ar, yh * ar, -yh, yh, zNear, zFar);

		return {{viewport, projMatrix, worldMatrix()}};
	}

	auto viewerPose = m_frame->getViewerPose();

	Vector<CameraView> cameraViews(2);

	for (uint eye = 0; eye < 2; ++eye) {
		auto& vview = viewerPose->views[eye];
		auto& cview = cameraViews[eye];
		cview.cameraMatrix = worldMatrix() * vview.transform;
		cview.projectionMatrix = vview.projectionMatrix;
		cview.viewport = vview.viewport;
	}

	return cameraViews;
}

} // namespace sgf
