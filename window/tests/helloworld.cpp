
#include "window/window.hh"

#include <opengl/opengl.hh>

using namespace sgf;

int main() {

	auto window = new GLWindow("Hello World", 1280, 720);

	glClearColor(.25f, 0, 1, 1);

	window->run([] { glClear(GL_COLOR_BUFFER_BIT); });
}
