#pragma once

#include "std.h"

namespace sgf {

//! Convert asset path to filesystem path
String resolveAssetPath(CString assetPath);

//! Load raw data from a filesystem path.
Vector<uchar> loadData(CString path);

//! Load a string from a filesystem path.
String loadString(CString path);

}
