#include "graphicsdevice.h"

#include "glgraphicsdevice.h"

#include <window/glwindow.h>

namespace sgf {

GraphicsDevice* createGraphicsDevice(Window* window) {
	assert(!GraphicsDevice::g_graphicsDevice && window && window->instanceOf<GLWindow>());

	GraphicsDevice::g_graphicsDevice = new GLGraphicsDevice(window->cast<GLWindow>());

	return GraphicsDevice::g_graphicsDevice;
}

}
