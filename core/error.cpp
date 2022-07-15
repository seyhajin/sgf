#include "error.h"

#ifdef OS_EMSCRIPTEN

#include "emscripten.h"
#include "emscripten/html5.h"

EM_JS(void, jsalert, (const char* msg_cstr), {
	const msg = UTF8ToString(msg_cstr);
	alert(msg);
	//	throw msg;
});

#endif

namespace sgf {

void alert(CString msg) {
	std::cout << msg << std::endl;
#ifdef OS_EMSCRIPTEN
	jsalert(msg.c_str());
#endif
}

void fail(CString msg, const char* file, int line) {

	alert( msg + "\n" + file + ": " + std::to_string(line));

#ifdef RELEASE
	abort();
#endif

#ifdef WIN32
	__debugbreak();
#else
	__builtin_trap();
#endif
}

}
