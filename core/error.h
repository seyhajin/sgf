#pragma once

#include "std.h"

namespace sgf {

void alert(CString message);

//! Abort with message and source info
[[noreturn]] void fail(CString cond, const char* file, int line);

//! Abort with message
#define panic(C) fail(C, __FILE__, __LINE__);

//! Release mode assert
#define verify(C)                                                                                                      \
	if (!(C)) fail(#C, __FILE__, __LINE__);
}
