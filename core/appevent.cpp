#include "appevent.h"

#include <condition_variable>
#include <deque>
#include <mutex>

namespace sgf {

namespace {

struct AppEvent {
	AppEventFunc func;
	void* context=nullptr;
};

std::deque<AppEvent> g_deque;

std::mutex g_dequeMutex;
std::condition_variable g_dequeCondVar;

} // namespace

// Post event from an external thread to main thread
void postAppEvent(AppEventFunc func, void* context) {
	{
		std::lock_guard<std::mutex> lock(g_dequeMutex);

		g_deque.push_back({std::move(func), context});
	}
	g_dequeCondVar.notify_one();
}

// Discard events with context
void discardAppEvents(void* context) {
	{
		std::lock_guard<std::mutex> lock(g_dequeMutex);

		for (auto& e : g_deque) {

			if (e.context == context) e.func = {};
		}
	}
}

void pollAppEvents() {

	uint n;
	{
		std::lock_guard<std::mutex> lock(g_dequeMutex);
		n = g_deque.size();
	}
	while (n--) {
		AppEvent ev;
		{
			std::lock_guard<std::mutex> lock(g_dequeMutex);
			ev = g_deque.front();
			g_deque.pop_front();
		}
		if (ev.func) ev.func();
	}
}

void waitAppEvents() {

	uint n;
	{
		std::unique_lock<std::mutex> lock(g_dequeMutex);
		g_dequeCondVar.wait(lock, [] { return !g_deque.empty(); });
		n = g_deque.size();
	}
	while (n--) {
		AppEvent ev;
		{
			std::lock_guard<std::mutex> lock(g_dequeMutex);
			ev = g_deque.front();
			g_deque.pop_front();
		}
		if (ev.func) ev.func();
	}
}

} // namespace sgf
