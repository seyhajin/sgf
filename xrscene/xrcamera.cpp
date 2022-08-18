#include "xrcamera.h"

#include <new>

#include <scene/scene.h>
#include <webxr/xrsystem.h>
#include <window/mouse.h>
#include <window/window.h>

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
		Recti viewport{0, scene()->graphicsDevice->window->size()};

		float yh = std::tan(fovY * degreesToRadians) * zNear;
		float ar = float(viewport.width()) / float(viewport.height());
		auto projMatrix = Mat4f::frustum(-yh * ar, yh * ar, -yh, yh, zNear, zFar);

		// Attempt a coolio 'neos vr' style mouse->eye effect.
		//
		auto coords = scene()->graphicsDevice->window->mouse()->position();
		Vec2f tcoords = coords - viewport.origin();
		tcoords = tcoords / viewport.size() * 2 - 1;
		tcoords.y = -tcoords.y;

		auto tv = projMatrix.inverse() * Vec4f(tcoords, 0, 1);

		Mat3f r;
		r.k = (tv.xyz() / tv.w).normalized();
		r.j = Vec3f{0,1,0};
		r.i = r.j.cross(r.k).normalized();
		r.j = r.k.cross(r.i).normalized();

		return {{viewport, projMatrix, {r, position()}}};
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
