#include <imgui/imgui.hh>
#include <scene/scene.hh>
#include <window/window.hh>

using namespace sgf;

Window* window;
GraphicsDevice* device;
Scene* scene;
Camera* camera;

RigidBody* createBallBody(float radius, float mass, CPhysicsMaterial physmat, Material* material) {

	auto body = new RigidBody();
	body->setInverseMass(1 / mass);
	body->setInverseInertiaTensor(hollowSphereInertiaTensor(mass, radius).inverse());
	body->setLinearDamping(.9f);
	body->setAngularDamping(.9f);

	auto collider = new SphereCollider(radius);
	collider->material = physmat;
	collider->setParent(body);

	auto model = createSphereModel(radius, 64, 32, material, false);
	model->setParent(body);

	return body;
}

Model* createRoomModel(float size, Material* material) {

	auto mesh = createBoxMesh(Boxf(-size / 2, 0, -size / 2, size / 2, size, size / 2), material);
	flipMesh(mesh);

	auto model = new Model(mesh);

	auto collider = new MeshCollider(mesh);
	collider->setParent(model);

	return model;
}

int main() {

	window = createWindow("Hello Box!", 1280, 720);

	device = createGraphicsDevice(window);

	// ***** Initialize scene *****
	//
	scene = new Scene(device);
	scene->directionalLightVector = Vec3f(0, -.5f, -1).normalized();
	scene->directionalLightColor = Vec3f(1);
	scene->clearColor = Vec4f(0, .25, .5f, 1);

	// Create camera.
	camera = new PerspectiveCamera();
	camera->setPosition({0, 1.0f, -1.9f});
	camera->enable();

	// Create light.
	auto light = new Light();
	light->setPosition({0, 20, 0});
	light->intensity = 10;
	light->enable();

	auto green = createMatteMaterial({.25f, .5f, .25f, 1});
	auto yellow = createMatteMaterial({.25f, .5f, .25f, 1});
	auto brown = createMatteMaterial({.75f, .5f, .125f, 1});

	auto bballmat = loadMatteMaterial("bball.png");

	// Create room
	auto room = createRoomModel(4, brown);
	room->enable();

	// ***** Begin main loop *****
	//
	ImGuiEx::CreateContext(window);

	window->run([bballmat] {
		ImGuiEx::NewFrame();

		if (window->keyboard()->key(SGF_KEY_UP).down()) {
			camera->translate({0, 0, .05f});
		} else if (window->keyboard()->key(SGF_KEY_DOWN).down()) {
			camera->translate({0, 0, -.05f});
		}

		if (window->keyboard()->key(SGF_KEY_A).down()) {
			camera->translate({0, .05f, 0});
		} else if (window->keyboard()->key(SGF_KEY_Z).down()) {
			camera->translate({0, -.05f, 0});
			auto pos = camera->position();
			pos.y = std::max(pos.y, .1213f);
			camera->setPosition(pos);
		}

		if (window->keyboard()->key(SGF_KEY_LEFT).down()) {
			camera->rotate({0, pi / 128.0f, 0});
		} else if (window->keyboard()->key(SGF_KEY_RIGHT).down()) {
			camera->rotate({0, -pi / 128.0f, 0});
		}

		if (window->keyboard()->key(SGF_KEY_SPACE).hit()) {

			auto bball = createBallBody(.1213f, .5903f, {.62f, .8f}, bballmat);

			bball->setMatrix(camera->worldMatrix());
			bball->enable();
			bball->addImpulse(bball->worldRotationMatrix().k * 2.5f, {0});
		}

		scene->update();
		scene->render();

		ImGuiEx::Render();
	});
}
