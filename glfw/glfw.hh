#pragma once

#if OS_WINDOWS
#include <windows.h>
#undef min
#undef max
#endif

#include <GLFW/glfw3.h>

#ifndef OS_EMSCRIPTEN
#if OS_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32 1
#define GLFW_EXPOSE_NATIVE_WGL 1
#elif OS_LINUX
#define GLFW_EXPOSE_NATIVE_X11 1
#define GLFW_EXPOSE_NATIVE_GLX 1
#endif

#include <GLFW/glfw3native.h>

#endif
