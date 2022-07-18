#pragma once

#include <core/core.hh>

namespace sgf {

struct FetchResponse {
	int httpStatus;
	String url;
	String data;
	String error;
};
using CFetchResponse = const FetchResponse&;

Promise<FetchResponse> fetch(CString url);

}
