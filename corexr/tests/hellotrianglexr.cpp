#if OS_EMSCRIPTEN

#include <core3d/core3d.hh>
#include <corexr/corexr.hh>
#include <glwindow/glwindow.h>

#include <emscripten.h>

using namespace sgf;

GraphicsContext* gc;

void renderFrame(double millis, XRFrame* frame) {

	frame->session->requestFrame(renderFrame);

	auto pose = frame->getViewerPose();

	if(!pose) return;

	//debug() << "### Camera:" << pose->transform;
	//debug() << "### Eyes:" << pose->views[0].transform << pose->views[1].transform;
	//debug() << "### Viewports:" << pose->views[0].viewport << pose->views[1].viewport;

	for(uint eye=0;eye<2;++eye) {

		auto&view = pose->views[eye];

		gc->setFrameBuffer(frame->session->frameBuffer());

		gc->setViewport(view.viewport);

		gc->clear(Vec4f(1,eye,0,1));
	}


#if 0
	const session = frame.session; // frame is a frame handling object - it's used to get frame sessions, frame WebGL layers and some more things
	session.requestAnimationFrame(onSessionFrame); // we simply set our animation frame function to be this function again
	let pose = frame.getViewerPose(xrRefSpace); // gets the pose of the headset, relative to the previously gotten referance space

	if(pose) { // if the pose was possible to get (if the headset responds)
		let glLayer = session.renderState.baseLayer; // get the WebGL layer (it contains some important information we need)

		gl.bindFramebuffer(gl.FRAMEBUFFER, glLayer.framebuffer); // sets the framebuffer (drawing target of WebGL) to be our WebXR display's framebuffer
		gl.clearColor(0.4, 0.7, 0.9, 1.0);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT); // clears the framebuffer (in the next episode we'll implement our ezgfx renderer here - for now, let's just use vanilla WebGL2, as we're not doing anything else than clearing the screen)
		for(let view of pose.views) { // we go through every single view out of our camera's views
			let viewport = glLayer.getViewport(view); // we get the viewport of our view (the place on the screen where things will be drawn)
			gl.viewport(viewport.x, viewport.y, viewport.width, viewport.height); // we set our viewport appropriately

			// Here we will draw our scenes
		}
	}
#endif

	debug() << "Rendering frame!";
}

int main() {

	new GLWindow("Skirmish 2022!", 640, 480);

	new GLGraphicsDevice();
	gc = graphicsDevice()->createGraphicsContext();

	new WebXRSystem();

	xrSystem()->isSessionSupported() | [](bool supported) {
		debug() << ">>> Session supported:" << supported;
		if (!supported) std::exit(1);

		xrSystem()->requestSession() | [](XRSession* session) { //
			debug() << "Session created!";
			session->requestFrame(renderFrame);
		};
	};

	emscripten_set_main_loop([] {}, 0, 1);
}

#endif
