#include "debug.h"

#include <chrono>
#include <ctime>

#ifdef COMPILER_MSVC
#pragma warning(disable : 4996)
#endif

namespace sgf {

String debugTimestamp(std::chrono::time_point<std::chrono::system_clock> now) {

	char buf[80];
	auto time = std::chrono::system_clock::to_time_t(now);
	auto localTime = localtime(&time);
	std::strftime(buf, sizeof(buf), "%F %H:%M:%S", localTime);

	char tbuf[120];
	ulong millis = (now.time_since_epoch().count() / 1000000) % 1000;
	std::snprintf(tbuf, sizeof(tbuf), "%s:%03d", buf, (int)millis);

	return tbuf;
}

String debugTimestamp() {
	return debugTimestamp(std::chrono::system_clock::now());
}

void defaultDebugOutputFunc(CString str) {
	std::puts(str.c_str());
	fflush(stdout);
}

thread_local DebugOutputFunc debugOutputFunc(&defaultDebugOutputFunc);

DebugStream::DebugStream(const char* file, int line) {
	String label = debugTimestamp() + " [" + file + ":" + std::to_string(line) + "] : ";
	m_rep = new Rep(std::chrono::system_clock::now(), file, line);
}

DebugStream::~DebugStream() {
	String timestamp = debugTimestamp(m_rep->time);
	String fileinfo = String("[") + m_rep->file + ":" + std::to_string(m_rep->line) + "]";
	String debug = m_rep->buf.str();
	debugOutputFunc(timestamp+" : "+debug);//+" "+fileinfo);
	delete m_rep;
}

} // namespace sgf
