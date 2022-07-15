#include "opengl.hh"

namespace sgf {

namespace{

Set<String> g_exts;

}

void initGLContext() {
#ifndef USE_OPENGLES
	glewInit();
#endif
	GLint n;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	for (int i = 0; i < n; ++i) {
		const char* p = (const char*)glGetStringi(GL_EXTENSIONS, i);
		g_exts.insert(p);
	}
}

bool checkGLExtension(CString ext) {
	return g_exts.find(ext)!=g_exts.end();
}

} // namespace sgf
