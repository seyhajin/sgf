
#include <window/window.hh>
#include <xrscene/xrscene.hh>

using namespace sgf;

Scene* scene;
XRCamera* camera;

void renderFrame(double time, XRFrame* frame) {

	frame->session->requestFrame(&renderFrame);

	scene->frameBuffer = frame->session->frameBuffer();

	mainWindow()->beginFrame();
	camera->beginFrame(frame);

	scene->update();
	scene->render();

	camera->endFrame();
	mainWindow()->endFrame();
}

void createScene() {

	// Create window.
	createMainWindow("Hello Box XR!", 1280, 720);

	// Create graphics device.
	createGraphicsDevice(mainWindow());

	// Create scene.
	scene = new Scene(graphicsDevice());
	scene->clearColor = Vec4f(.9f, .9f, .7f, 1);
	scene->ambientLightColor = Vec3f(0);
	scene->directionalLightColor = Vec3f(0);

	// Create camera.
	camera = new XRCamera();
	camera->enable();

	// Create dining room light.
	auto light = new Light();
	light->setPosition({0, 2, -2});
	light->color = {1, 0, 0};
	light->enable();

	// Create model instance.
	auto material = new Material();
	material->emissiveTexture = loadTexture("images/clion.png");
	auto mesh = createBoxMesh(.64f, .38f, .05f, material);
	auto model = new Model();
	model->renderData=createModelRenderData(mesh);
	model->color = {.1f, .1f, .1f, 1};
	model->translate({0, 0, 1});
	model->rotate({0, 0, 0});
	model->enable();
}

int main() {

	createScene();

	auto xrSystem = createXRSystem(graphicsDevice());

	xrSystem->requestSession() | [](XRSession* session) {
		if (!session) {
			debug() << "Failed to create session, make sure headset is active before launching app.";
			exit(1);
		}
		// Note: session->frameBuffer doesn't seem to be valid here...maybe move to XRFrame?
		// scene->frameBuffer = session->frameBuffer();
		session->requestFrame(&renderFrame);

		mainWindow()->stop();

		runAppEventLoop();
	};

	mainWindow()->run([] {
		scene->update();
		scene->render();
	});
}
