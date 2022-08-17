
#include <window/window.hh>
#include <xrscene/xrscene.hh>

using namespace sgf;

GLWindow* window;
GraphicsDevice* device;
Scene* scene;
XRCamera* camera;

void renderFrame(double time,XRFrame* frame) {

	frame->session->requestFrame(&renderFrame);

	scene->update();

	camera->beginFrame(frame);

	scene->frameBuffer = frame->session->frameBuffer();

	scene->render();

	camera->endFrame();
}

int main() {

	window = new GLWindow("Hello Box XR!", 1280, 720);

	device = new GLGraphicsDevice(window);

	// ***** Initialize scene *****
	//
	scene = new Scene(device);
	scene->clearColor = Vec4f(1, .25, .5f, 1);
	scene->ambientLightColor = Vec3f(0, 0, 0);
	scene->directionalLightVector = Vec3f(1, 1, -1).normalized();
	scene->directionalLightColor = Vec3f(0);

	// ***** Create camera *****
	//
	camera = new XRCamera(scene);
	//auto camera = new PerspectiveCamera(scene);
	camera->setPosition({0, 1, -3});
	camera->lookAt({0, 0, 0});
	camera->enable();

	// ***** Some nice RGB lights *****
	//
	float d = 2;
	float t = std::sqrt(d);
	float z = -2;
	float i = 8;
	Light* light{};

	light = new Light(scene);
	light->setPosition({0, d, z});
	light->color = {1, 0, 0};
	light->intensity = i;
	light->enable();

	light = new Light(scene);
	light->setPosition({-t, -t, z});
	light->color = {0, 1, 0};
	light->intensity = i;
	light->enable();

	light = new Light(scene);
	light->setPosition({t, -t, z});
	light->color = {0, 0, 1};
	light->intensity = i;
	light->enable();

	// ***** Create model instance *****
	//
	auto mesh = createBoxMesh(1, 1, 1, matteMaterial(Vec4f(1)));
	//auto mesh = createSphereMesh(1, 128, 64, matteMaterial(Vec4f(1)));
	auto model = createModel(mesh);
	auto inst = new ModelInstance(scene);
	inst->model = model;
	inst->color = {1, 1, 1, 1};
	inst->enable();

	// ***** Add collider *****
	//
	// auto collider = new SphereCollider(scene);
	// collider->radius = 1;
	// inst->addChild(collider);

	auto xrSystem = createXRSystem(device);

	xrSystem->requestSession() | [](XRSession* session) {
		if(!session) {
			debug() << "Failed to create session";
			exit(1);
		}
		// Note: session->frameBuffer doesn't seem to be valid here...maybe move to XRFrame?
		// scene->frameBuffer = session->frameBuffer();
		session->requestFrame(&renderFrame);
	};

	runAppEventLoop();

	window->run([=]{
		scene->update();
		scene->render();
	});

//	runAppEventLoop();

}
