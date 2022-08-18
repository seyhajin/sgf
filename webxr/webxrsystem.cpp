#ifdef OS_EMSCRIPTEN

#include "webxrsystem.h"

#include "emscripten.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"
#define EMSCRIPTEN_CALLBACK extern "C" EMSCRIPTEN_KEEPALIVE

namespace sgf {

#if 0
struct XRView {
	0   : AffineMat4f transform; (48)
	48  : Mat4f projectionMatrix; (64)
	112 : Recti viewport; (16)
	128	: sizeof
};

struct XRViewerPose {
	0   : AffineMat4f transform; (48)
	48  : XRView views[2]; (256)
	304	: sizeof
} ;

struct XRHandPose {
	0	: AffineMat4f transform; (48)
	48	: sizeof
};

#endif

namespace {

constexpr auto c_localSpace = "localSpace";

void flipZ(AffineMat4f& t) {
	t.m.i.z = -t.m.i.z;
	t.m.j.z = -t.m.j.z;
	t.m.k.x = -t.m.k.x;
	t.m.k.y = -t.m.k.y;
	t.t.z   = -t.t.z;
}

void flipZ(Mat4f& p) {
	p.k = -p.k;
}

} // namespace

// ***** WebXRSession::getHandPoses *****

// clang-format off
EM_JS(void, sgfXRGetHandPoses, (XRSession * session_ptr, XRFrame* frame_ptr, const char* localSpace_ptr, XRHandPose* handPoses_ptr), {

	const session = _sgfGetObject(session_ptr);
	const frame = _sgfGetObject(frame_ptr);
	const localSpace = _sgfGetObject(localSpace_ptr);

	const sources = session.inputSources;

	for (let i = 0; i < Math.min(sources.length, 2); ++i) {

		const pose = frame.getPose(sources[i].gripSpace, localSpace);

		if (pose) {
			const ptr = (handPoses_ptr + i*48) / 4;
			const matrix = pose.transform.matrix;
			for(let i = 0; i < 3; ++i) {
				HEAPF32[ptr + i * 3 + 0] = matrix[i * 4 + 0];
				HEAPF32[ptr + i * 3 + 1] = matrix[i * 4 + 1];
				HEAPF32[ptr + i * 3 + 2] = matrix[i * 4 + 2];
			}
			HEAPF32[ptr +  9] = matrix[12];
			HEAPF32[ptr + 10] = matrix[13];
			HEAPF32[ptr + 11] = matrix[14];
		}
	}
});
// clang-format on

const XRHandPose* WebXRFrame::getHandPoses() {

	sgfXRGetHandPoses(session, this, c_localSpace, m_handPoses);

	flipZ(m_handPoses[0].transform);
	flipZ(m_handPoses[1].transform);

	return m_handPoses;
}

// ***** WebXRFrame::getViewerPose *****

// clang-format off
EM_JS(XRViewerPose*, sgfXRGetViewerPose, (XRFrame* frame_ptr, XRViewerPose* viewer_ptr, const char* localSpace_ptr),{

	const frame = _sgfGetObject(frame_ptr);
	const localSpace = _sgfGetObject(localSpace_ptr);

	const viewer = frame.getViewerPose(localSpace);
//	console.log("### sfgGetViewerPos frame:", frame, "localSpace:", localSpace, "viewerPose:", viewer);

	if(!viewer) return 0;

	// camera matrix
	let ptr = viewer_ptr / 4;
	let matrix = viewer.transform.matrix;
	for(let i = 0; i < 3; ++i) {
		HEAPF32[ptr + i * 3 + 0] = matrix[i * 4 + 0];
		HEAPF32[ptr + i * 3 + 1] = matrix[i * 4 + 1];
		HEAPF32[ptr + i * 3 + 2] = matrix[i * 4 + 2];
	}
	HEAPF32[ptr +  9] = matrix[12];
	HEAPF32[ptr + 10] = matrix[13];
	HEAPF32[ptr + 11] = matrix[14];

	const layer = frame.session.renderState.baseLayer;

	for(let eye = 0; eye < 2; ++eye) {

		const view = viewer.views[eye];
		const view_ptr = viewer_ptr + 48 + eye * 128;

		// eye matrix
		ptr = view_ptr / 4;
		matrix = view.transform.matrix;
		for(let i = 0; i < 3; ++i) {
			HEAPF32[ptr + i * 3 + 0] = matrix[i * 4 + 0];
			HEAPF32[ptr + i * 3 + 1] = matrix[i * 4 + 1];
			HEAPF32[ptr + i * 3 + 2] = matrix[i * 4 + 2];
		}
		HEAPF32[ptr +  9] = matrix[12];
		HEAPF32[ptr + 10] = matrix[13];
		HEAPF32[ptr + 11] = matrix[14];

		// projection matrix
		ptr = (view_ptr + 48) / 4;
		matrix = view.projectionMatrix;
		for(let i = 0; i < 16; ++i) {
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
	return viewer_ptr;
});
// clang-format on

const XRViewerPose* WebXRFrame::getViewerPose() {

	auto viewerPose = sgfXRGetViewerPose(this, &m_viewerPose, c_localSpace);
	if (!viewerPose) {
		debug() << "### sgfXRGetViewerPose() returned nullptr";
		return nullptr;
	}
	flipZ(viewerPose->transform);

	for (uint eye = 0; eye < 2; ++eye) {
		auto& view = viewerPose->views[eye];
		flipZ(view.projectionMatrix);
		flipZ(view.transform);
	}

	return viewerPose;
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

	session.requestAnimationFrame((time, frame) => {

		_sgfRegisterObject(frame, frame_ptr);

		_sgfXRRequestFrameReady(func_ptr, frame_ptr, time);

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
EM_JS(GLuint, sgfXRGetFrameBuffer, (XRSession* session_ptr, Vec2i* size_ptr),{

	const session = _sgfGetObject(session_ptr);
	const layer = session.renderState.baseLayer;
	HEAP32[size_ptr / 4] = layer.framebufferWidth;
	HEAP32[size_ptr / 4 + 1] = layer.framebufferHeight;

	const buffer = layer.framebuffer;

	if(!buffer.name) {
		const name = GL.getNewId(GL.framebuffers);
		GL.framebuffers[name] = buffer;
		buffer.name = name;
	}
	return buffer.name;
});
// clang-format on

FrameBuffer* WebXRSession::frameBuffer() {

	Vec2i size;

	auto glFramebuffer = sgfXRGetFrameBuffer(this, &size);

	if (!m_frameBuffer || m_frameBuffer->width != size.x || m_frameBuffer->height != size.y ||
		m_frameBuffer->glFramebuffer != glFramebuffer) {
		m_frameBuffer = new GLFrameBuffer(graphicsDevice(), nullptr, nullptr, size.x, size.y, glFramebuffer);
	}

	return m_frameBuffer;
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
		// enable VR button on index.html page: webxr
		// requires manual input to create xr sessions.
		const vrButton = document.getElementById("vrbutton");
		vrButton.style.display = "block";

		vrButton.addEventListener("click", target => {

			navigator.xr.requestSession("immersive-vr").then(session => {

				_sgfRegisterObject(session, session_ptr);

				Module.ctx.makeXRCompatible().then(() => {

					const state = {baseLayer: new XRWebGLLayer(session, Module.ctx), depthNear: .1, depthFar: 100.0};

					session.updateRenderState(state);

					session.addEventListener("end", () => {});

					session.requestReferenceSpace("local").then(localSpace => {

						_sgfRegisterObject(localSpace, localSpace_ptr);

						_sgfXRRequestSessionResolved(promise_ptr, session_ptr);
					});
				});
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

#endif
