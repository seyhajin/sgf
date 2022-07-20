#include <fetch/fetch.hh>

#ifdef OS_EMSCRIPTEN
#include <emscripten.h>
#endif

int main() {

	sgf::fetch("https://www.httpvshttps.com") | [](sgf::CFetchResponse r){

		sgf::debug() << "Http status:" << r.httpStatus;
		sgf::debug() << "Text:" << r.text;

		sgf::debug() << "All done!";

#ifndef OS_EMSCRIPTEN
		std::exit(0);
#endif
	};

#ifdef OS_EMSCRIPTEN
	emscripten_set_main_loop([]{}, 0, true);
#else
	for (;;) {
		sgf::debug() << "Waiting for app events";
		sgf::waitAppEvents();
	}
#endif
}
