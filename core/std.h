#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstring>

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

#ifdef OS_EMSCRIPTEN
#include <emscripten.h>
#define EM_CALLBACK extern "C" EMSCRIPTEN_KEEPALIVE
#define EM_EXTERN extern "C"
#endif

#ifdef COMPILER_MSVC
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#endif

// TODO: Move me
#define SGF_ASSERT_ALIGNED16(T) static_assert((sizeof(T) & 15) == 0);

namespace sgf {

using ::std::nullptr_t;

using uchar = unsigned char;
#if defined(OS_EMSCRIPTEN) || defined(OS_WINDOWS)
using ulong = unsigned long;
using ushort = unsigned short;
using uint = unsigned int;
#endif

constexpr float pi = 3.14159265359f;
constexpr float twoPi = pi * 2.0f;
constexpr float halfPi = pi * .5f;
constexpr float degreesToRadians = .01745329252f;
constexpr float radiansToDegrees = 57.295779513f;
constexpr float unitLengthEpsilon = 1e-6f;		// 1/1000 of a millimeter (micrometer?)

using String = std::string;
using CString = const String&;

template <class T> using Vector = std::vector<T>;
template <class T> using CVector = const Vector<T>&;

template <class T> using Set = std::set<T>;
template <class T> using CSet = const Set<T>&;

template <class K, class V, class C = std::less<K>> using Map = std::map<K, V, C>;
template <class K, class V, class C = std::less<K>> using CMap = const Map<K, V, C>&;

template <class V> using StringMap = Map<String, V>;
template <class V> using CStringMap = const StringMap<V>&;

template <class C, class V> bool contains(const C& container, const V& value) {
	return std::find(container.begin(), container.end(), value) != container.end();
}

template <class C, class V> bool remove(C& container, const V& value) {
	auto it = std::find(container.begin(), container.end(), value);
	if (it == container.end()) return false;
	container.erase(it);
	return true;
}

//! return -1 if value < 0, 0 if value ==0, or 1 if value > 0.
template <class T> T sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

//! Replace all occurances of a substring.
String replace(CString string, CString find, CString replace);

//! Split a string into an array of substrings.
Vector<String> split(CString string, CString separator);

//! Convert string to uppercase.
String toUpper(CString string);

//! Convert string to uppercase.
String toLower(CString string);

//! Return true if string starts with substr.
bool startsWith(CString string, CString substr);

//! Return true if string ends with substr.
bool endsWith(CString string, CString substr);

//! True if we're on the main thread.
bool mainThread();

} // namespace sgf
