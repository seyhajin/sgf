#include "window.h"

#include "glwindow.h"

namespace sgf {

Window* createWindow(CString title, uint width, uint height) {
	return new GLWindow(title,width,height);
}

}
