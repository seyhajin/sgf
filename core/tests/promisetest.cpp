
#include <core/core.hh>

using namespace sgf;

Promise<bool> delay(uint millis) {
	return Promise<bool>(true);
}

int main() {

	auto fun = [](bool) {
		debug() << "Done!";
		return true;
	};

	delay(1000)
		.then([](bool) {
			debug() << "Done!";
			return true;
		})
		.then([](bool) {
			debug() << "Alrighty!";
			return false;
		});
}
