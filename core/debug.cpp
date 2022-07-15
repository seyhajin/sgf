#include "debug.h"

#include <ctime>
#include <chrono>

#ifdef COMPILER_MSVC
#pragma warning(disable:4996)
#endif

namespace sgf {

String debugTimestamp() {

	auto now = std::chrono::system_clock::now();

	std::time_t timer;
	timer = std::chrono::system_clock::to_time_t(now);
	auto timeinfo = localtime(&timer);

	ulong millis = (now.time_since_epoch().count() / 1000000) % 1000;

	char buf[80];
	std::strftime(buf, sizeof(buf), "%F %H:%M:%S", timeinfo);
	std::snprintf(buf, sizeof(buf),"%s:%03d", buf, (int)millis);

	return buf;
}

void defaultDebugOutputFunc(CString str) {

	std::puts(str.c_str());
	fflush(stdout);
}

thread_local DebugOutputFunc debugOutputFunc(&defaultDebugOutputFunc);

} // namespace sgf
