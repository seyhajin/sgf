#include <glwindow/glwindow.hh>
#include <scene/scene.hh>

using namespace sgf;

GLWindow* window;
GraphicsDevice* device;
Scene* scene;

int main() {

	window = new GLWindow("Hello Box!", 1280, 720);

	device = new GLGraphicsDevice(window);

	// ***** Initialize scene *****
	//
	scene = new Scene(device);
	scene->clearColor = Vec4f(0, .25, .5f, 1);
	scene->ambientLightColor = Vec3f(0, 0, 0);
	scene->directionalLightVector = Vec3f(1, 1, -1).normalized();
	scene->directionalLightColor = Vec3f(0);

	// ***** Create camera *****
	//
	auto camera = new PerspectiveCamera(scene);
	// camera = new OrthographicCamera(scene);
	camera->setPosition({0, 0, -2});
	//	camera->lookAt({0, 0, 0});
	camera->enable();

	// ***** RGB Create lights *****
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

	// ***** Create collider *****
	//
	// auto collider = new SphereCollider(scene);
	// collider->radius = 1;
	// inst->addChild(collider);

	// ***** Create model instance *****
	//
	// auto mesh = createBoxMesh(1, 1, 1, matteMaterial(Vec4f(1)));
	auto mesh = createSphereMesh(1, 128, 64, matteMaterial(Vec4f(1)));
	auto model = createModel(mesh);
	auto inst = new ModelInstance(scene);
	inst->model = model;
	inst->color = {1, 1, 1, 1};
	inst->enable();

	// ***** Create collider *****
	//
	auto collider = new SphereCollider(scene);
	collider->radius = 1;
	inst->addChild(collider);

	// ***** Begin main loop *****
	//
	window->run([camera, inst] {
		if (auto collider = scene->intersectEyeRay(window->mouse()->position(), 0)) { debug() << "### BANG!"; }

		inst->rotate({0, .01f, 0});

		scene->update();

		scene->render();
	});
}
