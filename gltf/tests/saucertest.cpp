#include <gltf/gltf.hh>
#include <scene/scene.hh>
#include <window/window.hh>
#include <imgui/imgui.hh>

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
	auto camera = new PerspectiveCamera();
	camera->translate({0, 1, -2.5f});
	camera->lookAt({0,0,0});
	camera->enable();

	// Create light, move it up and back a bit.
	auto light = new Light();
	light->translate({0, 2, -.5f});
	light->enable();

	// load model, leave it at the origin.
	auto model = loadModel("saucer.glb", scene, true);
	model->enable();

	// Begin 'main loop'
	ImGuiEx::CreateContext(window);

	window->run([model] {
		ImGuiEx::NewFrame();

		auto mousePos = window->mouse()->position();

		auto collider = scene->intersectEyeRay(mousePos, 0);
		ImGuiEx::Debug() << (collider ? "YES!" : "NO");

		model->rotate({0,.1f,0});

		scene->update();
		scene->render();

		ImGuiEx::Render();
	});
}
