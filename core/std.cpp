
#include "std.h"

#if COMPILER_MSVC
#include <cfloat>
#endif

#include <thread>

#ifdef ASAN_ENABLED
// The mere presence of this magic bit of code causes debugger to popup when address sanitizer detects corruption at runtime.
extern "C" const char* __asan_default_options() { // NOLINT (unused function)
	return "abort_on_error=1:detect_leaks=0";
}
#endif

namespace sgf {

namespace {

using Clock = std::chrono::system_clock;

const Clock::time_point g_timerStart = Clock::now();

std::thread::id g_mainThreadId = std::this_thread::get_id();

Vector<String> g_appArgs;

} // namespace

String replace(CString str, CString find, CString rep) {
	auto r = str;
	for (size_t pos = 0; pos < r.size();) {
		pos = r.find(find, pos);
		if (pos == String::npos) break;
		r.replace(pos, find.size(), rep, 0, rep.size());
		pos += rep.size();
	}
	return r;
}

Vector<String> split(CString str, CString sep) {
	Vector<String> fields;
	for (size_t i = 0; i < str.size();) {
		size_t e = str.find(sep, i);
		if (e == String::npos) {
			fields.push_back(str.substr(i));
			break;
		}
		fields.push_back(str.substr(i, e - i));
		i = e + sep.size();
	}
	return fields;
}

String join(CVector<String> fields, CString sep) {
	if (fields.empty()) return {};
	String str;
	for (const auto& field : fields) {
		if (!str.empty()) str += sep;
		str += field;
	}
	return str;
}

String toUpper(CString cstr) {
	auto str = cstr;
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
	return str;
}

String toLower(CString cstr) {
	auto str = cstr;
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
	return str;
}

bool startsWith(CString string, CString substr) {
	return string.size() >= substr.size() && string.compare(0, substr.size(), substr) == 0;
}

bool endsWith(CString string, CString substr) {
	return string.size() >= substr.size() && string.compare(string.size() - substr.size(), substr.size(), substr) == 0;
}

bool mainThread() {
	return std::this_thread::get_id() == g_mainThreadId;
}

void enableNaNExceptions() {
#if COMPILER_MSVC
	uint current;
	_controlfp_s(&current, 0, _EM_INVALID);
#endif
}

void disableNaNExceptions() {
#if COMPILER_MSVC
	uint current;
	_controlfp_s(&current, _EM_INVALID, _EM_INVALID);
#endif
}

void sgfMain(int argc, const char* argv[]) {
	g_appArgs.resize(argc);
	for (int i = 0; i < argc; ++i) { //
		g_appArgs[i] = argv[i];
	}
	enableNaNExceptions();
}

CVector<String> appArgs() {
	return g_appArgs;
}

int64_t nanoseconds() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - g_timerStart).count();
}

int64_t microseconds() {
	return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - g_timerStart).count();
}

int64_t milliseconds() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - g_timerStart).count();
}

int64_t seconds() {
	return std::chrono::duration_cast<std::chrono::seconds>(Clock::now() - g_timerStart).count();
}

} // namespace sgf
