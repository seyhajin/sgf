
#include "glwindow/glwindow.hh"

#include <opengl/opengl.hh>

using namespace sgf;

int main() {

	auto window = new GLWindow("Skirmish 2022!", 1280, 720);

	glClearColor(.25f, 0, 1, 1);

	window->run([] { glClear(GL_COLOR_BUFFER_BIT); });
}
