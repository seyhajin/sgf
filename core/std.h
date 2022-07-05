#pragma once

#include <cassert>
#include <cmath>
#include <cstring>

#include <algorithm>
#include <any>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#endif

#define SGF_ALIGN4 __attribute__((aligned(4)))
#define SGF_ALIGN8 __attribute__((aligned(8)))
#define SGF_ALIGN16 __attribute__((aligned(16)))

#define SGF_ASSERT_ALIGNED16(T) static_assert((sizeof(T) & 15) == 0);

namespace sgf {

using uchar = unsigned char;
#if defined(OS_EMSCRIPTEN) || defined(OS_WINDOWS)
using ulong = unsigned long;
using ushort = unsigned short;
using uint = unsigned int;
#endif

constexpr float pi = 3.14159265359f;
constexpr float twoPi = pi * 2.0f;
constexpr float halfPi = pi * .5f;
constexpr float degreesToRadians = .0174533f;

using String = std::string;
using CString = const String&;

template <class T> using Vector = std::vector<T>;
template <class T> using CVector = const Vector<T>&;

template <class K, class V, class C = std::less<K>> using Map = std::map<K, V, C>;
template <class K, class V, class C = std::less<K>> using CMap = const Map<K, V, C>&;

template <class V> using StringMap = Map<String, V>;
template <class V> using CStringMap = const StringMap<V>&;

//! Erase first matching element from a vector
template <class T, class U> bool erase(Vector<T>& vec, const U& value) {
	auto it = std::find(vec.begin(), vec.end(), value);
	if (it == vec.end()) return false;
	vec.erase(it);
	return true;
}

#if 0
template <class T, class U> bool addFirst(Vector<T>& vec, const U& value) {
	if (std::find(vec.begin(), vec.end(), value) != vec.end()) return false;
	vec.push_back(value);
	return true;
}

template <class T, class U> bool contains(CVector<T> vec, const U& value) {
	return std::find(vec.begin(), vec.end(), value) != vec.end();
}

//! Convert a value to a string, using the value type's operator<<(std::ostream&).
template <class T> inline String toString(const T& value) {
	std::stringstream buf;
	buf << value;
	return buf.str();
}
#endif

template <class T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

[[noreturn]] void fail(CString cond, const char* file, int line);

//! Release mode assert
#define verify(C)                                                                                                      \
	if (!(C)) fail(#C, __FILE__, __LINE__);

//! Abort with message
#define panic(C) fail(C, __FILE__, __LINE__);

// Seed rnd generator
void seedRnd(uint seed);

// Seed rnd generator with current time().
void seedRnd();

//! Generate a pseudo random number from [0, 1).
float rnd();

//! Generate a pseudo random number from [0, max).
float rnd(float max);

//! Generate a pseudo random number from [min, max).
float rnd(float min, float max);

//! Load a string from a file.
String loadString(CString path);

//! Split a string into an array of substrings.
Vector<String> splitString(CString string, CString separator);

//! Load bytes from a file.
Vector<uchar> loadData(CString path);

//! convert string to uppercase.
String toUpper(String str);

//! convert string to uppercase.
String toLower(String str);

} // namespace sgf

// Release mode asserts, remove for final release...
#undef assert
#define assert verify
