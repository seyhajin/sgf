#include "std.h"

#ifdef ASAN_ENABLED
// The mere presence of this magic bit of code causes debugger to popup when address sanitizer detects corruption at runtime.
extern "C" const char* __asan_default_options() { // NOLINT (unused function)
	return "abort_on_error=1:detect_leaks=0";
}
#endif

namespace sgf {

String stringReplace(CString str, CString find, CString rep) {
	auto r = str;
	for (size_t pos = 0; pos < r.size();) {
		pos = r.find(find, pos);
		if (pos == String::npos) break;
		r.replace(pos, find.size(), rep, 0, rep.size());
		pos += rep.size();
	}
	return r;
}

Vector<String> stringSplit(CString str, CString sep) {
	Vector<String> bits;
	for (size_t i0 = 0; i0 < str.size();) {
		size_t i = str.find(sep, i0);
		if (i != String::npos) {
			bits.push_back(str.substr(i0, i - i0));
			i0 = i + sep.size();
			continue;
		}
		bits.push_back(str.substr(i0));
		break;
	}
	return bits;
}

String StringtoUpper(CString cstr) {
	auto str=cstr;
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
	return str;
}

String StringtoLower(CString cstr) {
	auto str=cstr;
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
	return str;
}

} // namespace sgf
