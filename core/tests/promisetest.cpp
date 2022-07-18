
#include <core/core.hh>

using namespace sgf;

template <class T> Duration<T> duration(T seconds) {
	return Duration<T>(seconds);
}

Promise<bool> delay(double seconds) {

	Promise<bool> promise;

	Thread thread([promise, seconds]() mutable {

		debug() << "sleeping...";

		ThisThread::sleep_for(duration(seconds));

		debug() << "done!";

		promise.resolve(true);
	});

	thread.detach();

	return promise;
}

int main() {

	delay(1)
		.then([](bool) {
			debug() << "Done1!";
			return delay(.5f);
		})
		.then([](bool) {
			debug() << "Done2";
			return delay(.25f);
		})
		.then([](bool) {
			debug() << "Done3!";
		});

	for (;;) {
		debug() << "Waiting for app events";
		waitAppEvents();
	}
}
