
#include <window/window.hh>
#include <xrscene/xrscene.hh>

using namespace sgf;

GLWindow* window;
GraphicsDevice* device;
Scene* scene;
XRCamera* camera;

void renderFrame(double time, XRFrame* frame) {

	frame->session->requestFrame(&renderFrame);

	scene->frameBuffer = frame->session->frameBuffer();

	window->beginFrame();
	camera->beginFrame(frame);

	scene->update();
	scene->render();

	camera->endFrame();
	window->endFrame();
}

void createScene() {

	// Create window.
	window = new GLWindow("Hello Box XR!", 1280, 720);

	// Create graphics device.
	device = new GLGraphicsDevice(window);

	// Create scene.
	scene = new Scene(device);
	scene->clearColor = Vec4f(.9f, .9f, .7f, 1);
	scene->ambientLightColor = Vec3f(0);
	scene->directionalLightColor = Vec3f(0);

	// Create camera.
	camera = new XRCamera(scene);
	camera->enable();

	// Create dining room light.
	auto light = new Light(scene);
	light->setPosition({0, 2, -2});
	light->color = {1, 0, 0};
	light->enable();

	// Create model instance.
	auto mesh = createBoxMesh(.64f, .38f, .05f, matteMaterial(Vec4f(1)));
	auto model = createModel(mesh);
	auto inst = new ModelInstance(scene);
	inst->model = model;
	inst->color = {.1f, .1f, .1f, 1};
	inst->translate({0, -.14f, .78f});
	inst->rotate({.2f, 0, 0});
	inst->enable();
}

int main() {

	createScene();

	auto xrSystem = createXRSystem(device);

	xrSystem->requestSession() | [](XRSession* session) {
		if (!session) {
			debug() << "Failed to create session, make sure headset is active before launching app.";
			exit(1);
		}
		// Note: session->frameBuffer doesn't seem to be valid here...maybe move to XRFrame?
		// scene->frameBuffer = session->frameBuffer();
		session->requestFrame(&renderFrame);

		window->stop();

		runAppEventLoop();
	};

	window->run([]{
		scene->update();
		scene->render();
	});
}
