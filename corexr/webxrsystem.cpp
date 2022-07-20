#include "webxrsystem.h"

#include <emscripten.h>

namespace sgf {

namespace {

extern "C" {

EMSCRIPTEN_KEEPALIVE void onXRSessionSupported(Promise<bool>* promise_ptr, bool supported){

	promise_ptr->resolve(supported);
	delete promise_ptr;
}

EMSCRIPTEN_KEEPALIVE void onXRSessionStarted(Promise<XRSession*>* promise_ptr,XRSession* session_ptr) {

	promise_ptr->resolve(session_ptr);
	delete promise_ptr;
}

EMSCRIPTEN_KEEPALIVE void onXRRequestSessionError(Promise<XRSession*>* promise_ptr,XRSession* session_ptr) {

	promise_ptr->resolve(nullptr);
	delete promise_ptr;
	delete session_ptr;
}

// clang-format off
EM_JS(void, sgfXRIsSessionSupported, (Promise<bool>* promise_ptr), {

	if(!navigator.xr) {
		_onXRSessionSupported(promise_ptr, false);
		return;
	}

	navigator.xr.isSessionSupported("immersive-vr").then(supported => {
		_onXRSessionSupported(promise_ptr, supported);
	});
});

EM_JS(void, sgfXRRequestSession, (Promise<XRSession*>* promise_ptr, XRSession* session_ptr), {

	if(!navigator.xr) {
		onRequestSessionError(promise_ptr, session_ptr);
		return;
	}

	navigator.xr.isSessionSupported("immersive-vr").then(supported => {
		if(!supported) {
			_sgfRequestSessionError(promise_ptr, session_ptr);
			return;
		}
		navigator.xr.requestSession("immersive-vr").then(session => {
				sgfRegisterObject(session, session_ptr);
				_sgfXRSessionStarted(promise_ptr, session_ptr);
		});
	});
});

// clang-format on
}

}

// ***** WebXRFrame *****
//
const XRViewerPose* WebXRFrame::getViewerPose() {
}

// ***** WebXRSession *****

Promise<XRFrame*> WebXRSession::requestFrame() {
}

// ***** WebXRSystem *****

Promise<bool> WebXRSystem::isSessionSupported() {

	auto promise_ptr = new Promise<bool>;
	sgfXRIsSessionSupported(promise_ptr);
	return *promise_ptr;
}

Promise<XRSession*> WebXRSystem::requestSession() {

	auto session_ptr = new WebXRSession();
	auto promise_ptr = new Promise<XRSession*>();
	sgfXRRequestSession(promise_ptr,session_ptr);
	return *promise_ptr;
}

}
