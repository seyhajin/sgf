#include "fileio.h"
#include "error.h"

#include <fstream>
#include <sstream>
#include <sys/stat.h>

#if OS_WINDOWS
#include <windows.h>
#include <direct.h>
#endif

namespace sgf {

namespace {

#if OS_EMSCRIPTEN
const String assetsDir = "/";
#else
const String assetsDir = "./assets/";
#endif

} // namespace

String resolveAssetPath(CString assetPath) {
	return assetsDir + assetPath;
}

Vector<uint8_t> loadData(CString path) {
	std::ifstream ifs(path, std::ios::binary | std::ios::ate);
	if (!ifs.is_open()) panic("Failed to open file '" + path + "'");
	auto size = ifs.tellg();
	std::vector<uint8_t> data(size);
	ifs.seekg(0, std::ios::beg);
	ifs.read((char*)data.data(), size);
	return data;
}

String loadString(CString path) {

	std::ifstream fs(path, std::ios::binary | std::ios::ate);
	if (!fs.is_open()) panic("Failed to open file '" + path + "'");

	auto size = fs.tellg();
	String str(size, '\0'); // construct string to stream size
	fs.seekg(0);

	if (fs.read(&str[0], size)) return str;

	panic("!!! Failed to load string from '"+path+"'");

	return "";
}

void saveString(CString str, CString path) {

	std::ofstream fs(path, std::ios::binary | std::ios::trunc);
	if (!fs.is_open()) panic("Failed to open file '" + path + "'");

	if(fs.write(&str[0], std::streamsize(str.size()))) return;

	panic("!!! Failed save string to '"+path+"'");
}

#if OS_WINDOWS
void createDir(CString path) {
	_mkdir(path.c_str());
}

bool isFile(CString path) {
	struct _stat stat;
	_stat(path.c_str(),&stat);
	return stat.st_mode & _S_IFREG;
}

bool isDir(CString path) {
	struct _stat stat;
	_stat(path.c_str(),&stat);
	return stat.st_mode & _S_IFDIR;
}

void openURL(CString url) {
	CoInitializeEx( NULL,COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE );
	ShellExecute( HWND_DESKTOP,0,url.c_str(),0,0,SW_SHOWNORMAL );
}
#endif

#if OS_LINUX
void createDir(CString path) {
	assert(!mkdir(path.c_str(), 0777));
}

bool isFile(CString path) {
	struct stat st;
	assert(!stat(path.c_str(), &st));
	return st.st_mode & S_IFREG;
}

bool isDir(CString path) {
	struct stat st;
	assert(!stat(path.c_str(), &st));
	return st.st_mode & S_IFDIR;
}
#endif



} // namespace sgf
