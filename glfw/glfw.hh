#pragma once

#if OS_WINDOWS
#include <windows.h>
#undef min
#undef max
#endif

#if OS_LINUX
#include <opengl/opengl.hh>
#endif

#include <GLFW/glfw3.h>
