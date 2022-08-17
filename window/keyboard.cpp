#include "keyboard.h"

#include <glfw/glfw.hh>

#include "glwindow.h"

namespace sgf {

// ***** Keyboard *****

Keyboard::Keyboard(GLFWwindow* glfwWindow) : UIDevice(maxButtons) {

	glfwSetKeyCallback(glfwWindow, [](GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
		if (key >= maxButtons || (action != GLFW_PRESS && action != GLFW_RELEASE)) return;

		auto keyboard = GLWindow::getWindow(glfwWindow)->keyboard();
		keyboard->setButtonDown(key, action == GLFW_PRESS);
	});
}

} // namespace sgf
