#include "std.h"

#include <fstream>
#include <sstream>

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/html5.h"

EM_JS(void, jsalert, (const char* msg_cstr), {
	const msg = UTF8ToString(msg_cstr);
	alert(msg);
	//	throw msg;
});

#endif

namespace sgf {

namespace {

struct ranctx {
	uint a = 3;
	uint b = 11;
	uint c = 23;
	uint d = 37;
};

ranctx r;

#define rot(x, k) (((x) << (k)) | ((x) >> (32 - (k))))
uint rndval(ranctx* x) {
	uint e = x->a - rot(x->b, 27);
	x->a = x->b ^ rot(x->c, 17);
	x->b = x->c + x->d;
	x->c = x->d + e;
	x->d = e + x->a;
	return x->d;
}

void rndinit(ranctx* x, uint seed) {
	uint i;
	x->a = 0xf1ea5eed, x->b = x->c = x->d = seed;
	for (i = 0; i < 20; ++i) { (void)rndval(x); }
}

} // namespace

#ifdef ASAN_ENABLED
extern "C" const char* __asan_default_options() {
	return "abort_on_error=1:detect_leaks=0";
}
#endif

void seedRnd(uint seed) {
	rndinit(&r, seed);
}

void seedRnd() {
	uint tm = time(nullptr);
	seedRnd(tm);
}

float rnd() {
	return float(rndval(&r) & 0xffffff) / float(0x1000000);
}
float rnd(float max) {
	return rnd() * max;
}
float rnd(float min, float max) {
	return rnd(max - min) + min;
}

String loadString(CString path) {
	std::ifstream ifs(path);
	if (!ifs.is_open()) panic("Failed to open file '" + path + "'");
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	return buffer.str();
}

Vector<String> splitString(CString str, CString sep) {
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

Vector<uchar> loadData(CString path) {
	std::ifstream ifs(path, std::ios::binary | std::ios::ate);
	if (!ifs.is_open()) panic("Failed to open file '" + path + "'");
	auto size = ifs.tellg();
	std::vector<uchar> data(size);
	ifs.seekg(0, std::ios::beg);
	ifs.read((char*)data.data(), size);
	return data;
}

void fail(CString msg, const char* file, int line) {

	String lineInfo = String(file) + ": " + std::to_string(line);

#if __EMSCRIPTEN__
	jsalert((msg + "\n" + lineInfo).c_str());
#else
	std::cout << "!!! " << msg << std::endl;
	std::cout << "!!! " << lineInfo << std::endl;
#endif

#ifdef WIN32
	__debugbreak();
#else
	__builtin_trap();
#endif
}

String toUpper(String str) {
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
	return str;
}

String toLower(String str) {
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
	return str;
}

} // namespace sgf
