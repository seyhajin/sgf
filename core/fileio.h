#pragma once

#include "std.h"

namespace sgf {

//! Convert asset path to filesystem path
String resolveAssetPath(CString assetPath);

//! Load raw data from a file.
Vector<uchar> loadData(CString path);

//! Load a string from a file.
String loadString(CString path);

}
