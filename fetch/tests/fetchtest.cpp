#include <fetch/fetch.hh>

int main() {

	sgf::fetch("https://www.google.com").then([](sgf::CFetchResponse r){

		sgf::debug() << "Http status:" << r.httpStatus;
		sgf::debug() << "Error:" << r.error;
		sgf::debug() << "Data:" << r.data;

		std::exit(0);
	});

	for(;;) sgf::waitAppEvents();
}
