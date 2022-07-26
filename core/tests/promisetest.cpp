
#include <core/core.hh>

using namespace sgf;

namespace sgf {
using Thread = std::thread;
using CThread = const Thread&;
template <class T> using Duration = std::chrono::duration<T>;
namespace ThisThread = std::this_thread;
}

template <class T> sgf::Duration<T> duration(T seconds) {
	return sgf::Duration<T>(seconds);
}

sgf::Promise<bool> delay(double seconds) {

	sgf::Promise<bool> promise;

	sgf::Thread thread([promise, seconds]() mutable {
		sgf::debug() << "sleeping...";

		sgf::ThisThread::sleep_for(duration(seconds));

		sgf::debug() << "...awake!";

		promise.resolveAsync(true);
	});

	thread.detach();

	return promise;
}

int main() {

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

	for (;;) {
		sgf::debug() << "Waiting for app events";
		sgf::waitAppEvents();
	}
}
