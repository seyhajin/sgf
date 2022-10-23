#include <fetch/fetch.hh>

int main() {

	sgf::fetch("https://www.httpvshttps.com") | [](sgf::CFetchResponse r){

		sgf::debug() << "Http status:" << r.httpStatus;
		sgf::debug() << "Text:" << r.text;

		sgf::debug() << "All done!";

		std::exit(0);
	};

	sgf::runAppEventLoop();
}
