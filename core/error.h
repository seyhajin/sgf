#pragma once

#include "debug.h"
#include "std.h"

namespace sgf {

void alert(CString message);

//! Abort with message and source info
DebugStream panicex(CString message, const char* file = nullptr, int line = -1);

#define panic(MSG) panicex(#MSG, __FILE__, __LINE__)

} // namespace sgf
