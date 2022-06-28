#include "debug.h"

#include <chrono>

namespace sgf {

String debugTimestamp() {
	using std::chrono::system_clock;
	auto currentTime = std::chrono::system_clock::now();
	char buffer[80];

	auto transformed = currentTime.time_since_epoch().count() / 1000000;

	auto millis = transformed % 1000;

	std::time_t tt;
	tt = system_clock::to_time_t(currentTime);
	auto timeinfo = localtime(&tt);
	strftime(buffer, 80, "%F %H:%M:%S", timeinfo);
	sprintf(buffer, "%s:%03d", buffer, (int)millis);

	return buffer;
}

void defaultDebugOutputFunc(CString str) {
	fflush(stdout);
	std::puts(str.c_str());
	fflush(stdout);
}

thread_local DebugOutputFunc debugOutputFunc(&defaultDebugOutputFunc);

} // namespace sgf
