#pragma once

#include <core/core.hh>

namespace sgf {

struct FetchResponse {
	int httpStatus;
	String text;
	String error;
};
using CFetchResponse = const FetchResponse&;

Promise<FetchResponse> fetch(CString url);

}
