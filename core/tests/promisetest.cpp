#include <core/core.hh>

#include <thread>

using namespace sgf;

Promise<bool> delay(double seconds) {

	Promise<bool> promise;

	std::thread thread([promise, seconds]() mutable {

		debug() << "Main thread"<<mainThread();

		debug() << "sleeping...";

		std::this_thread::sleep_for(std::chrono::duration<double>(seconds));

		debug() << "...awake!";

		promise.resolveAsync(true);
	});

	thread.detach();

	return promise;
}

int main() {

	debug() << "Main thread"<<mainThread();

	delay(2) | [](bool) {
		sgf::debug() << "Done1!";
		return delay(1);
	} | [](bool) {
		sgf::debug() << "Done2!";
		return delay(.5f);
	} | [](bool) {
		sgf::debug() << "Done3!";
		return delay(.25f);
	} | [](bool) {
		sgf::debug() << "Done4!";
		return delay(.125f);
	} | [](bool) {
		sgf::debug() << "Bye!";
		std::exit(0);
	};

	runAppEventLoop();
}
