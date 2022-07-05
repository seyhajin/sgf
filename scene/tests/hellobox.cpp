#include <glwindow/glwindow.hh>
#include <scene/scene.hh>

using namespace sgf;

namespace {

GLWindow* window;
GraphicsDevice* device;
Scene* scene;
Camera* camera;
ModelRenderer* boxRenderer;
ModelInstance* boxInstance;

} // namespace

int main() {

	window = new GLWindow("Hello Box!", 1280, 720);

	device = new GLGraphicsDevice();

	// ***** Initialize scene *****
	//
	scene = new Scene(device);
	scene->clearColor = Vec4f(0, .25, .5f, 1);
	scene->ambientLightColor = Vec3f(0, 0, 0);
	scene->directionalLightVector = Vec3f(1, 1, -1).normalized();
	scene->directionalLightColor = Vec3f(1);

	// ***** Initialize camera *****
	//
	camera = new PerspectiveCamera();
	// camera = new OrthographicCamera();
	window->sizeChanged.connect([](CVec2i size) { //
		camera->viewport = Recti(Vec2i(0), size);
	});
	camera->viewport = Recti(Vec2i(0), window->size());
	camera->setPosition(Vec3f(0, 1, -1));
	camera->lookAt(Vec3f(0, 0, 0));
	scene->addCamera(camera);

	// ***** Add new box renderer to scene *****
	//
	auto boxMesh = createBoxMesh(1, 1, 1, matteMaterial(Vec4f(1)));
	auto boxModel = createModel(boxMesh);
	boxRenderer = new ModelRenderer(boxModel);
	scene->addRenderer(boxRenderer);

	// ***** Add new box instance to box renderer *****
	//
	boxInstance = new ModelInstance();
	boxRenderer->addInstance(boxInstance);
	boxInstance->color = Vec4(1, 1, 0, 1);

	// ***** Begin main loop *****
	//
	window->run([] {
		boxInstance->rotate({.007f, .01f, 0});
		scene->render(window->size());
	});
}
