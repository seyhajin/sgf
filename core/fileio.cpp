#include "fileio.h"
#include "error.h"

#include <fstream>
#include <sstream>

namespace sgf {

namespace {

#ifdef OS_EMSCRIPTEN
const String assetsDir = "/";
#else
const String assetsDir = "./assets/";
#endif

} // namespace

String resolveAssetPath(CString assetPath) {
	return assetsDir + assetPath;
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

String loadString(CString path) {
	std::ifstream ifs(path);
	if (!ifs.is_open()) panic("Failed to open file '" + path + "'");
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	return buffer.str();
}

} // namespace sgf
