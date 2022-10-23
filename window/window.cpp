#include "window.h"

#include "glwindow.h"

namespace sgf {

Window* createMainWindow(CString title, uint width, uint height) {
	if(Window::g_mainWindow) panic("Main window already created");

	Window::g_mainWindow = new GLWindow(title,width,height);

	return Window::g_mainWindow;
}

}
