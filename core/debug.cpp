#include "debug.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef COMPILER_MSVC
#pragma warning(disable : 4996)
#endif

namespace sgf {

using Clock = std::chrono::steady_clock;
//using Clock = std::chrono::system_clock;
//using Clock = std::chrono::high_resolution_clock;

static String debugTimeStamp(const Clock::time_point& tp) {

	auto dur= tp.time_since_epoch();
	auto durS = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
	auto durMs = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

	std::ostringstream str;
	str << std::put_time(std::localtime(&durS),"%Y-%m-%d %H:%M:%S.");
	str << std::setw(3) << std::setfill('0') << int(durMs - durS * 1000);

	return str.str();
}

String debugTimeStamp() {
	return debugTimeStamp(Clock::now());
}

void defaultDebugOutputFunc(CString str) {
	std::puts(str.c_str());
	fflush(stdout);
}

thread_local DebugOutputFunc debugOutputFunc(&defaultDebugOutputFunc);

DebugStream::DebugStream(DebugOutputFunc outputFunc, String separator)
	: m_rep(new Rep(std::move(outputFunc), std::move(separator))) {
}

DebugStream::~DebugStream() {
	if (!m_rep) return;
	m_rep->outputFunc(m_rep->buf.str());
	delete m_rep;
}

DebugStream debug(const char* file, int line) {
	return {[file, line, now = debugTimeStamp()](CString str) {
		String fileinfo = file ? (String("[") + file + ":" + std::to_string(line) + "]") : String();
		debugOutputFunc(now + " : " + str); //+" "+fileinfo);
	}};
}

} // namespace sgf
