#pragma once

#if OS_EMSCRIPTEN
#include "fetch_emscripten.h"
#else
#include "fetch_libcurl.h"
#endif
