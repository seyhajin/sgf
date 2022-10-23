
#include "window/window.hh"

#include <opengl/opengl.hh>

using namespace sgf;

int safemod(int x, int y) {
	assert(y>0);
	return x >= 0 ? (x % y) : y - (-x % y);
};

#include <stdio.h>

int main() {

	createMainWindow("Hello World", 1280, 720);

	puts((char*)glGetString(GL_VERSION));
	puts((char*)glGetString(GL_VENDOR));
	puts((char*)glGetString(GL_RENDERER));

	GLint n;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&n);
	debug() << "###"<<n<<"!";
//	exit(0);

	glClearColor(.25f, 0, 1, 1);

	mainWindow()->run([] { glClear(GL_COLOR_BUFFER_BIT); });
}
