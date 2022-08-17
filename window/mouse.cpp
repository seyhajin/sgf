#include "mouse.h"

#include "glwindow.h"

#include <glfw/glfw.hh>

namespace sgf {

Mouse::Mouse(GLFWwindow* glfwWindow) : UIDevice(maxButtons) {

	glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* glfwWindow, double x, double y) {
		auto mouse = GLWindow::getWindow(glfwWindow)->mouse();
		mouse->m_position = {float(x), float(y)};
	});

	glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* glfwWindow, int button, int action, int mods) {
		if(button>=maxButtons) return;

		auto mouse = GLWindow::getWindow(glfwWindow)->mouse();
		mouse->setButtonDown(button, action == GLFW_PRESS);
	});

	glfwSetScrollCallback(glfwWindow, [](GLFWwindow* glfwWindow, double x, double y) {
		auto mouse = GLWindow::getWindow(glfwWindow)->mouse();
		mouse->m_scroll = {float(x), float(y)};
	});
}

} // namespace sgf
