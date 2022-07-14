
#include "openxr/openxrcamera.h"
#include <glwindow/glwindow.hh>
#include <openxr/openxr.hh>
#include <scene/scene.hh>

using namespace sgf;

namespace {

GLWindow* window;
GraphicsDevice* device;
Scene* scene;
OpenXRCamera* camera;
ModelRenderer* boxRenderer;
ModelInstance* boxInstance;
OpenXRSession* xrSession;

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

	// ***** Add new box renderer to scene *****
	//
	auto boxMesh = createBoxMesh(.1f, .1f, .1f, matteMaterial(Vec4f(1)));
	auto boxModel = createModel(boxMesh);
	boxRenderer = new ModelRenderer(boxModel);
	scene->addRenderer(boxRenderer);

	// ***** Add new box instance to box renderer *****
	//
	boxInstance = new ModelInstance();
	boxRenderer->addInstance(boxInstance);
	boxInstance->color = Vec4(1, 1, 0, 1);
	boxInstance->translate({0, 0, .25f});

	xrSession = new OpenXRSession(window);

	// ***** Initialize VR camera *****
	//
	camera = new OpenXRCamera(xrSession);
	scene->addCamera(camera);

	debug() << "###" << xrSession->swapchainTextureSize() << camera->viewport;

	// ***** Begin main loop *****
	//
	window->run([] {

		boxInstance->rotate({.007f, .01f, 0});

		xrSession->pollEvents();

		if (xrSession->beginFrame()) {
			scene->render(camera->viewport.value().size());
			xrSession->endFrame();
		}
	});
}
