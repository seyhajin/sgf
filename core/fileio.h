#pragma once

#include "std.h"

namespace sgf {

//! Convert asset path to filesystem path.
String resolveAssetPath(CString assetPath);

//! Load raw data from a filesystem path.
Vector<uchar> loadData(CString path);

//! Save raw data from to filesystem path.
void saveData(CVector<uchar> data, CString path);

//! Load a string from a filesystem path.
String loadString(CString path);

//! Write a string to filesystem path.
void saveString(CString string, CString path);

void openURL(CString url);

} // namespace sgf
