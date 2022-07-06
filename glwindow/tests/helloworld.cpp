
#include "glwindow/glwindow.hh"

#include <opengl/opengl.hh>

using namespace sgf;

int main() {

	uint width = 1280;
	uint height = 720;

	auto window = new GLWindow("Skirmish 2022!", width, height);

	window->shouldClose.connect(window, &GLWindow::close);

	glClearColor(.25f, 0, 1, 1);

	window->run([] { glClear(GL_COLOR_BUFFER_BIT); });
}
