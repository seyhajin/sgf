#include <glwindow/glwindow.hh>
#include <scene/scene.hh>

using namespace sgf;

int main() {

	uint width = 1280;
	uint height = 720;

	auto window = new GLWindow("Skirmish 2022!", width, height);

#ifndef USE_OPENGLES
	auto debugFunc = [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
						const void* userParam) { debug() << "OpenGL Debug:" << message; };
	glDebugMessageCallback(debugFunc, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
#endif

	new GLGraphicsDevice();

	new Scene();

	activeScene()->clearColor = Vec4f(0, 1, .5f, 1);
	activeScene()->ambientLightColor = Vec3f(1, 1, 0);

	auto camera = new Camera();

	camera->viewport = Recti(0, 0, width, height);
	camera->setEnabled(true);

	camera->position() += camera->rotation().k * -5;
	camera->updateWorldMatrix();

	debug() << "### camera:" << camera->worldMatrix();

	float yaw = 0;

	window->run([width, height, &yaw] {
		activeScene()->debugRenderer()->clear();
		activeScene()->debugRenderer()->modelMatrix = Mat3f::rotation({yaw += .01f, yaw / 2l, 0});
		activeScene()->debugRenderer()->addBox(Boxf(-1, 1));
		activeScene()->render(Vec2i(width, height));
	});
}
