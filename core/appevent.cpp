#include "appevent.h"

#include <condition_variable>
#include <deque>
#include <mutex>

#if OS_EMSCRIPTEN
#include "emscripten/threading.h"
#endif

namespace sgf {

namespace {

struct AppEvent {
	AppEventFunc func;
	void* context = nullptr;
};

std::deque<AppEvent> g_deque;
std::mutex g_dequeMutex;

// True if running blocking app event loop.
// False if polling.
bool g_blockingMode = false;

// True if dispatching events inside pollAppEvents.
// Should only be used by main thread.
bool g_pollingAppEvents = false;

#if OS_EMSCRIPTEN
EM_CALLBACK void notifyMainThread() {
	pollAppEvents();
}
#else
std::condition_variable g_dequeCondVar;
#endif

} // namespace

// Post event from an external thread to main thread
void postAppEvent(AppEventFunc func, void* context) {
	{
		std::lock_guard<std::mutex> lock(g_dequeMutex);
		g_deque.push_back({std::move(func), context});
	}

	// Non-blocking apps must manually invoke pollAppEvents() periodically.
	if(!g_blockingMode) return;

	// If we're already on main thread and inside a pollAppEvent loop no need to notify main thread.
	if(mainThread() && g_pollingAppEvents) return;

#if OS_EMSCRIPTEN
	emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_V, &notifyMainThread);
#else
	g_dequeCondVar.notify_one();
#endif
}

// Discard events with context
void discardAppEvents(void* context) {
	std::lock_guard<std::mutex> lock(g_dequeMutex);
	for (auto& e : g_deque) {
		if (e.context == context) e.func = {};
	}
}

void pollAppEvents() {
	assert(mainThread() && !g_pollingAppEvents);

	g_pollingAppEvents = true;

	for (;;) {
		AppEvent ev;
		{
			std::lock_guard<std::mutex> lock(g_dequeMutex);
			if (g_deque.empty()) break;
			ev = g_deque.front();
			g_deque.pop_front();
		}
		if (ev.func) ev.func();
	}

	g_pollingAppEvents=false;
}

void runAppEventLoop() {
	assert(mainThread() && !g_blockingMode);

	g_blockingMode=true;

#if OS_EMSCRIPTEN
	emscripten_exit_with_live_runtime();
#else
	for (;;) {
		{
			// Must use unique_lock for condvars apparently.
			std::unique_lock<std::mutex> lock(g_dequeMutex);
			g_dequeCondVar.wait(lock, [] { return !g_deque.empty(); });
		}
		pollAppEvents();
	}
#endif
}

} // namespace sgf
