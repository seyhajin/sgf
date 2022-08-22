#include <gltf/gltf.hh>
#include <scene/scene.hh>
#include <window/window.hh>

using namespace sgf;

Window* window;
GraphicsDevice* grdevice;
Scene* scene;

int main() {

	// Create window
	window = createWindow("Saucer test!", 1280, 720);

	// Create graphics device
	grdevice = createGraphicsDevice(window);

	// Create scene
	scene = new Scene(grdevice);

	// Create camera, move it back a bit.
	auto camera = new PerspectiveCamera(scene);
	camera->translate({0, 0, -2.5f});
	camera->enable();

	// Create light, move it up and back a bit.
	auto light = new Light(scene);
	light->translate({0, 1, -.5f});
	light->enable();

	// Create model, leave ait at the origin.
	auto mesh = loadMesh("saucer.glb");
	auto model = createModel(mesh);
	auto inst = new ModelInstance(scene);
	inst->model = model;
	inst->enable();

	// Begin 'main loop'
	window->run([] {
		scene->update();
		scene->render();
	});
}
