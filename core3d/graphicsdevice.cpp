#include "graphicsdevice.h"

#include "glgraphicsdevice.h"

#include <window/glwindow.h>

namespace sgf {

GraphicsDevice* createGraphicsDevice(Window* window) {
	assert(window->instanceOf<GLWindow>());

	return new GLGraphicsDevice(window->cast<GLWindow>());
}

}
