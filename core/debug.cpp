#include "debug.h"

#include <ctime>
#include <chrono>

#ifdef COMPILER_MSVC
#pragma warning(disable:4996)
#endif

namespace sgf {

String debugTimestamp() {

	auto now = std::chrono::system_clock::now();

	char buf[80];
	auto time = std::chrono::system_clock::to_time_t(now);
	auto localTime =  localtime(&time);
	std::strftime(buf, sizeof(buf), "%F %H:%M:%S", localTime);

	char tbuf[120];
	ulong millis = (now.time_since_epoch().count() / 1000000) % 1000;
	std::snprintf(tbuf, sizeof(tbuf),"%s:%03d", buf, (int)millis);

	return tbuf;
}

void defaultDebugOutputFunc(CString str) {

	std::puts(str.c_str());
	fflush(stdout);
}

thread_local DebugOutputFunc debugOutputFunc(&defaultDebugOutputFunc);

} // namespace sgf
