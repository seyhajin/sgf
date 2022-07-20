#pragma once

#include <core3d/core3d.hh>

namespace sgf {

struct XRView {
	CMat4f projectionMatrix;
	CAffineMat4f transform;
};

struct XRViewerPose {
	CAffineMat4f transform;
	XRView views[2];
};

class XRFrame : public Object{
public:
	virtual const XRViewerPose* getViewerPose() = 0;
};

class XRSession : public Object{
public:
	virtual Promise<XRFrame*> requestFrame() = 0;
};

class XRSystem : public Object{
public:
	XRSystem();

	virtual Promise<bool> isSessionSupported() = 0;

	virtual Promise<XRSession*> requestSession() = 0;

private:
	static inline XRSystem* g_instance;
	friend XRSystem* xrSystem();
};

XRSystem* xrSystem() {
	return XRSystem::g_instance;
}

} // namespace sgf
