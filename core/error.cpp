#include "error.h"

#if OS_EMSCRIPTEN
#include "emscripten.h"
#include "emscripten/html5.h"
EM_JS(void, jsalert, (const char* msg_cstr), {
	const msg = UTF8ToString(msg_cstr);
	alert(msg);
	//	throw msg;
});

#elif OS_WINDOWS
#include <windows.h>
#endif

namespace sgf {

void alert(CString msg) {

#if OS_EMSCRIPTEN
	jsalert(msg.c_str());
#elif OS_WINDOWS
	MessageBoxA(GetActiveWindow(), msg.c_str(), "Alert!", MB_OK);
#else
	std::cout << msg << std::endl;
#endif
}

DebugStream panicex(CString msg, const char* file, int line) {

	auto now = debugTimeStamp();

	return {

		[msg, file, line, now](CString str) {
			String fileinfo = file ? (String("[") + file + ":" + std::to_string(line) + "]") : String();

			auto dstr = now + ": " + msg;
			if (!str.empty()) dstr += "\n" + str;

			debugOutputFunc(dstr);

			alert(msg + "\n" + file + ": " + std::to_string(line));

#ifdef RELEASE
			abort();
#endif

#if OS_EMSCRIPTEN
			EM_ASM(debugger;);
#elif OS_WINDOWS
			__debugbreak();
#else
			__builtin_trap();
#endif
		}};
}

} // namespace sgf
