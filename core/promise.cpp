#include "promise.h"

#include <thread>

namespace sgf {

#ifndef OS_EMSCRIPTEN
Promise<bool> sleep(double seconds) {

	Promise<bool> promise;

	std::thread thread([promise, seconds]() mutable {
		std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
		promise.resolveAsync(true);
	});

	thread.detach();

	return promise;
}
#endif

}
