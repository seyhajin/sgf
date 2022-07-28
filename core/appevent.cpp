#include "appevent.h"

#include <condition_variable>
#include <deque>
#include <mutex>

namespace sgf {

namespace {

struct AppEvent {
	AppEventFunc func;
	void* context = nullptr;
};

std::deque<AppEvent> g_deque;

bool g_runningEventLoop;

#ifndef OS_EMSCRIPTEN
std::mutex g_dequeMutex;
std::condition_variable g_dequeCondVar;
#endif

} // namespace

// Post event from an external thread to main thread
void postAppEvent(AppEventFunc func, void* context) {
#ifdef OS_EMSCRIPTEN
	if(g_runningEventLoop) {
		// Need to set up an EventTarget and cause it to fire here I think.
		panic("OOPS");
	}
	g_deque.push_back({std::move(func), context});
#else
	{
		std::lock_guard<std::mutex> lock(g_dequeMutex);

		g_deque.push_back({std::move(func), context});
	}
	g_dequeCondVar.notify_one();
#endif
}

// Discard events with context
void discardAppEvents(void* context) {
	#ifndef OS_EMSCRIPTEN
	std::lock_guard<std::mutex> lock(g_dequeMutex);
	#endif

	for (auto& e : g_deque) {
		if (e.context == context) e.func = {};
	}
}

void pollAppEvents() {
#if OS_EMSCRIPTEN
	while(!g_deque.empty()) {
		auto& ev=g_deque.front();
		if(ev.func) ev.func();
		g_deque.pop_front();
	}
#else
	for(;;) {
		AppEvent ev;
		{
			std::lock_guard<std::mutex> lock(g_dequeMutex);
			if (g_deque.empty()) break;
			ev = g_deque.front();
			g_deque.pop_front();
		}
		if(ev.func) ev.func();
	}
#endif
}

void waitAppEvents() {
#ifdef OS_EMSCRIPTEN
	panic("OOPS");
#else
	while(g_deque.empty()) {
		std::unique_lock<std::mutex> lock(g_dequeMutex);
		g_dequeCondVar.wait(lock, [] { return !g_deque.empty(); });
	}
	pollAppEvents();
#endif
}

void runAppEventLoop() {
	g_runningEventLoop=true;
#if OS_EMSCRIPTEN
	emscripten_exit_with_live_runtime();
#else
	for (;;) waitAppEvents();
#endif
}

} // namespace sgf
