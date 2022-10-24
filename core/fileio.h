#pragma once

#include "std.h"

namespace sgf {

//! Convert asset path to filesystem path.
String resolveAssetPath(CString assetPath);

//! Load raw data from a filesystem path.
Vector<uint8_t> loadData(CString path);

//! Save raw data from to filesystem path.
void saveData(CVector<uint8_t> data, CString path);

//! Load a string from a filesystem path.
String loadString(CString path);

//! Write a string to filesystem path.
void saveString(CString string, CString path);

//! Create a directory, creating parents too if necessary
void createDir(CString dir);

//! True if path is a file
bool isFile(CString path);

//! True if path is a directory
bool isDir(CString path);

//! Open a URL in a browser
void openURL(CString url);

} // namespace sgf
