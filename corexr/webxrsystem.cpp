#include "webxrsystem.h"

#include <emscripten.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"
#define EMSCRIPTEN_CALLBACK extern "C" EMSCRIPTEN_KEEPALIVE

namespace sgf {

#if 0
struct XRView {
	0   : AffineMat4f transform; (48)
	48  : Mat4f projectionMatrix; (64)
	112 : Recti viewport; (16)
	128
};

struct XRViewerPose {
	0   : AffineMat4f transform; (48)
	48  : XRView views[2]; (256)
} ;
#endif

namespace {

constexpr auto c_localSpace = "localSpace";

}

// ***** WebXRFrame::getViewerPose *****

// clang-format off
EM_JS(void,sgfXRGetViewerPose,(XRFrame * frame_ptr, XRViewerPose* viewer_ptr,const char* localSpace_ptr),{

	const frame = _sgfGetObject(frame_ptr);
	const localSpace = _sgfGetObject(localSpace_ptr);

	const viewer = frame.getViewerPose(localSpace);

	// camera matrix
	let ptr = viewer_ptr / 4;
	let matrix = viewer.transform.matrix;
	for(let i=0; i < 4; ++i) {
		HEAPF32[ptr + i * 3 + 0] = matrix[i];
		HEAPF32[ptr + i * 3 + 1] = matrix[i + 4];
		HEAPF32[ptr + i * 3 + 2] = matrix[i + 8];
	}

	const layer = frame.session.renderState.baseLayer;

	for(let eye=0; eye<2; ++eye) {

		const view = viewer.views[eye];
		const view_ptr = viewer_ptr + 48 + eye * 128;

		// eye matrix
		ptr = view_ptr / 4;
		matrix = view.transform.matrix;
		for(let i=0; i < 4; ++i) {
			HEAPF32[ptr + i * 3 + 0] = matrix[i];
			HEAPF32[ptr + i * 3 + 1] = matrix[i + 4];
			HEAPF32[ptr + i * 3 + 2] = matrix[i + 8];
		}

		// projection matrix
		ptr = (view_ptr + 48) / 4;
		matrix = view.projectionMatrix;
		for(let i=0; i < 16; ++i) {
			HEAPF32[ptr+i] = matrix[i];
		}

		// viewport
		ptr = (view_ptr + 112) / 4;
		const vp = layer.getViewport(view);
		HEAP32[ptr+0] = vp.x;
		HEAP32[ptr+1] = vp.y;
		HEAP32[ptr+2] = vp.x + vp.width;
		HEAP32[ptr+3] = vp.y + vp.height;

	}
});
// clang-format on

const XRViewerPose* WebXRFrame::getViewerPose() {

	sgfXRGetViewerPose(this,&m_viewerPose,c_localSpace);

	return &m_viewerPose;
}

// ***** WebXRSession::requestFrame *****

EMSCRIPTEN_CALLBACK void sgfXRRequestFrameReady(XRFrameFunc* func_ptr, XRFrame* frame_ptr, double time) {

	(*func_ptr)(time, frame_ptr);

	delete frame_ptr;
	delete func_ptr;
}

// clang-format off
EM_JS(void, sgfXRRequestFrame, (XRSession* session_ptr, XRFrameFunc* func_ptr, XRFrame* frame_ptr),{

	const session = _sgfGetObject(session_ptr);

	session.requestAnimationFrame((millis, frame) => {

		_sgfRegisterObject(frame, frame_ptr);

		_sgfXRRequestFrameReady(func_ptr, frame_ptr);

		_sgfDeregisterObject(frame_ptr);
	});
});
// clang-format on

void WebXRSession::requestFrame(XRFrameFunc func) {

	auto func_ptr = new XRFrameFunc(func);

	auto frame_ptr = new WebXRFrame(this);

	sgfXRRequestFrame(this, func_ptr, frame_ptr);
}

// ***** WebXRSession::frameBuffer *****

// clang-format off
EM_JS(GLuint, sgfXRGetFrameBuffer, (XRSession* session_ptr),{

	const session = _sgfGetObject(session_ptr);
	const layer = session.renderState.baseLayer;
	return layer.Framebuffer;
});
// clang-format on

FrameBuffer* WebXRSession::frameBuffer() {

	auto glBuf = sgfXRGetFrameBuffer(this);
}

// ***** WebXRSystem::isSessionSupported *****

EMSCRIPTEN_CALLBACK void sgfXRIsSessionSupportedResolved(Promise<bool>* promise_ptr, bool supported) {
	promise_ptr->resolve(supported);
	delete promise_ptr;
}

// clang-format off
EM_JS(void, sgfXRIsSessionSupported, (Promise<bool>* promise_ptr), {
	if (!navigator.xr) {
		_sgfXRIsSessionSupportedResolved(promise_ptr, false);
		return;
	}
	navigator.xr.isSessionSupported("immersive-vr").then(supported => {
		_sgfXRIsSessionSupportedResolved(promise_ptr, supported);
	});
});
// clang-format on

Promise<bool> WebXRSystem::isSessionSupported() {
	auto promise_ptr = new Promise<bool>;
	sgfXRIsSessionSupported(promise_ptr);
	return *promise_ptr;
}

// ***** WebXRSystem::requestSession *****

EMSCRIPTEN_CALLBACK void sgfXRRequestSessionResolved(Promise<XRSession*>* promise_ptr, XRSession* session_ptr) {
	promise_ptr->resolve(session_ptr);
	delete promise_ptr;
}

EMSCRIPTEN_CALLBACK void sgfXRRequestSessionError(Promise<XRSession*>* promise_ptr, XRSession* session_ptr) {
	promise_ptr->resolve(nullptr);
	delete promise_ptr;
	delete session_ptr;
}

// clang-format off
EM_JS(void, sgfXRRequestSession, (Promise<XRSession*> * promise_ptr, XRSession* session_ptr, const char* localSpace_ptr), {
	if (!navigator.xr) {
		_sgfXRRequestSessionError(promise_ptr, session_ptr);
		return;
	}
	navigator.xr.isSessionSupported("immersive-vr").then(supported => {
		if (!supported) {
			_sgfXRRequestSessionError(promise_ptr, session_ptr);
			return;
		}
		navigator.xr.requestSession("immersive-vr").then(session => {
			_sgfRegisterObject(session, session_ptr);

			session.updateRenderState({baseLayer : new XRWebGLLayer(session, Module.ctx)});

			session.addEventListener("end", () => {});

			session.requestReferenceSpace("local").then(localSpace => {
				_sgfRegisterObject(localSpace, localSpace_ptr);
				_sgfXRRequestSessionResolved(promise_ptr, session_ptr);
			});
		});
	});
});
// clang-format on

Promise<XRSession*> WebXRSystem::requestSession() {

	auto session_ptr = new WebXRSession(this);
	auto promise_ptr = new Promise<XRSession*>();

	sgfXRRequestSession(promise_ptr, session_ptr, c_localSpace);

	return *promise_ptr;
}

} // namespace sgf

#pragma clang diagnostic pop
