#include <filesystem>
#include <string>

#define CPPHTTPLIB_NO_EXCEPTIONS 1
#include "libhttplib/httplib.h"

using String = std::string;
using CString = const String&;

void alert(CString msg) {
#if OS_EMSCRIPTEN
	jsalert(msg.c_str());
#elif OS_WINDOWS
	MessageBoxA(GetActiveWindow(), msg.c_str(), "Alert!", MB_OK);
#else
	std::cout << msg << std::endl;
#endif
}

bool startsWith(CString string, CString substr) {
	return string.size() >= substr.size() && string.compare(0, substr.size(), substr) == 0;
}

bool endsWith(CString string, CString substr) {
	return string.size() >= substr.size() && string.compare(string.size() - substr.size(), substr.size(), substr) == 0;
}

String dequote(CString str) {
	if (!startsWith(str, "\"") || !endsWith(str, "\"")) return str;
	return str.substr(1, str.size() - 2);
}

void startServer(CString root, CString host, int port) {
	std::thread([root, host, port] {
		httplib::Server server;
		server.set_mount_point("/", root);
		server.listen(host, port);
	}).detach();
}

void startBrowser(CString url) {
#if OS_WINDOWS
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	ShellExecute(HWND_DESKTOP, 0, url.c_str(), 0, 0, SW_SHOWNORMAL);
#endif
}

int interpreter() {

	String exepath;
	String execwd;

	for (;;) {

		puts("(gdb)");
		fflush(stdout);

		String cmd;
		if (!std::getline(std::cin, cmd)) return 0;

		String args;
		auto spc = cmd.find(' ');
		if (spc != String::npos) {
			args = cmd.substr(spc + 1);
			cmd = cmd.substr(0, spc);
		}

		if (cmd == "-exec-run") {

			auto i = exepath.rfind('\\');
			if (i == String::npos) i = exepath.rfind('/');
			if (i == String::npos) return 1;

			auto file = exepath.substr(i + 1);
			auto root = exepath.substr(0, i);
			auto url = "http://localhost:8080/" + file;

			startServer(root, "localhost", 8080);

			startBrowser(url);

		} else if (cmd == "-gdb-exit") {

			return 0;
		} else if (cmd == "-file-exec-and-symbols") {
			exepath = dequote(args);
		} else if (cmd == "-environment-cd") {
			execwd = dequote(args);
		} else if (cmd == "-gdb-set") {
		} else {
			// alert("cmd:" + cmd + " args:" + args);
		}
		puts("^done");
	}
	return 0;
}

int main(int argc, const char* argv[]) {

	if (argc == 2 && !std::strcmp(argv[1], "--version")) {
		puts("GNU gdb (GDB) 11.1");
		return 0;
	}

	if (argc == 2 && !std::strcmp(argv[1], "--interpreter=mi2")) { //
		return interpreter();
	}

	String str;
	for (int i = 0; i < argc; ++i) str += " " + String(argv[i]);
	alert("??? " + str);
	return 1;
}
