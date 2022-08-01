#pragma once

#include <core3d/core3d.hh>

namespace sgf {

class XRFrame;
class XRSession;
class XRSystem;

using XRFrameFunc = Function<void(double, XRFrame*)>;

struct XRView {
	AffineMat4f transform;
	Mat4f projectionMatrix;
	Recti viewport;
};

struct XRViewerPose {
	AffineMat4f transform;
	XRView views[2];
};

struct XRHandPose {
	AffineMat4f transform;
};

class XRFrame : public Object {
public:
	XRSession* const session;

	virtual const XRViewerPose* getViewerPose() = 0;

	virtual const XRHandPose* getHandPoses() = 0;

protected:
	XRFrame(XRSession* session) : session(session) {
	}
};

class XRSession : public Object {
public:
	XRSystem* const system;

	virtual void requestFrame(XRFrameFunc func) = 0;

	virtual FrameBuffer* frameBuffer() = 0;

protected:
	XRSession(XRSystem* system) : system(system) {
	}
};

class XRSystem : public Object {
public:
	virtual Promise<bool> isSessionSupported() = 0;

	virtual Promise<XRSession*> requestSession() = 0;

protected:
	XRSystem() {
		assert(!g_instance);
		g_instance = this;
	}

private:
	static inline XRSystem* g_instance;
	friend XRSystem* xrSystem() {
		return g_instance;
	}
};

XRSystem* xrSystem();

} // namespace sgf
