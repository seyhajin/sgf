#pragma once

#include "function.h"
#include "signal.h"

namespace sgf {

using AppEventFunc = Function<void()>;

// Can be called by any thread.
//
void postAppEvent(AppEventFunc event, void* context = nullptr);

// Can be called by any thread.
//
void discardAppEvents(void* context);

// Should only be called by main thread.
//
void pollAppEvents();

// Should only be called by main thread.
//
void runAppEventLoop();

} // namespace sgf
