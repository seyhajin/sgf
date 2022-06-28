#include "opengl.hh"

namespace sgf {

void initOpenGL() {
#ifndef USE_OPENGLES
	glewInit();
#endif
}

} // namespace sgf
