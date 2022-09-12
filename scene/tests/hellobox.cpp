#include <imgui/imgui.hh>
#include <scene/scene.hh>
#include <window/window.hh>

using namespace sgf;

Window* window;
GraphicsDevice* device;
Scene* scene;

int main() {

	window = createWindow("Hello Box!", 1280, 720);

	window->keyboard()->key(SGF_KEY_V).pressed.connect([] { window->vsyncEnabled = !window->vsyncEnabled; });
	window->keyboard()->key(SGF_KEY_F).pressed.connect([] { window->fullScreenMode = !window->fullScreenMode; });

	device = createGraphicsDevice(window);

	// ***** Initialize scene *****
	//
	scene = new Scene(device);
	scene->clearColor = Vec4f(0, .25, .5f, 1);
	//scene->ambientLightColor = Vec3f(0, 0, 0);
	scene->directionalLightVector = Vec3f(1, 1, -1).normalized();
	scene->directionalLightColor = Vec3f(0);

	// ***** Create camera *****
	//
	auto camera = new PerspectiveCamera();
	camera->setPosition({0, 0, -2.5f});
	camera->lookAt({0, 0, 0});
	camera->enable();

	// ***** RGB Create lights *****
	//
	float d = 2;
	float t = std::sqrt(d);
	float z = -2;
	float i = 8;
	Light* light{};

	light = new Light();
	light->setPosition({0, d, z});
	light->color = {1, 0, 0};
	light->intensity = i;
	light->enable();

	light = new Light();
	light->setPosition({-t, -t, z});
	light->color = {0, 1, 0};
	light->intensity = i;
	light->enable();

	light = new Light();
	light->setPosition({t, -t, z});
	light->color = {0, 0, 1};
	light->intensity = i;
	light->enable();

	// ***** Create model instance *****
	//
	auto mesh = createBoxMesh(1, 1, 1, new Material());
	auto model = new Model();
	model->renderData=createModelRenderData(mesh);
	model->enable();

	// ***** Create collider *****
	//
	auto collider = new MeshCollider();
	collider->colliderData = new MeshColliderData(mesh);
	//auto collider = new SphereCollider(scene);
	//collider->radius = 1;
	collider->setParent(model);

	//model->rotate({0, .1f, 0});

	// ***** Begin main loop *****
	//
	ImGuiEx::CreateContext(window);

	window->run([camera, model] {
		ImGuiEx::NewFrame();

		ImGui::Begin("Window settings");
		ImGuiEx::Checkbox("vsyncEnabled", window->vsyncEnabled);
		ImGuiEx::Checkbox("fullScreenMode", window->fullScreenMode);
		ImGui::End();

		ImGuiEx::Debug() << window->size();

		if (auto collider = scene->intersectEyeRay(window->mouse()->position(), 0.0001f)) { debug() << "### BANG!"; }

		scene->update();
		scene->render();

		ImGuiEx::Render();
	});
}
