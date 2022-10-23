#include "fetch_emscripten.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"

#ifndef OS_EMSCRIPTEN
#error OOPS
#endif

#include <emscripten.h>

namespace sgf {

extern "C" {

EMSCRIPTEN_KEEPALIVE void sgfOnFetchResponse(Promise<FetchResponse>* promise_ptr, int status, const char* text_cstr,
											 const char* textEnd_cstr, const char* error_cstr) {

	String text = textEnd_cstr ? String(text_cstr, textEnd_cstr) : String(text_cstr);

	promise_ptr->resolve({status, std::move(text), error_cstr ? String(error_cstr) : String()});

	delete promise_ptr;
}

// clang-format off
EM_JS(void,sgfFetch,(Promise<FetchResponse>* promise_ptr, const char* url_cstr),{

	const url = UTF8ToString(url_cstr);

	const options = {
		cache: "reload"
	};

	fetch(url, options).then(response => {
		if(response.arrayBuffer) {
			response.arrayBuffer().then(buffer => {
				const buf_ptr = _malloc(buffer.byteLength);
				HEAPU8.set(new Uint8Array(buffer), buf_ptr);
				_sgfOnFetchResponse(promise_ptr, response.status, buf_ptr, buf_ptr + buffer.byteLength, 0);
				_free(buf_ptr);
			});
		}else if(response.text) {
			response.text().then(text => {
				const text_cstr = allocateUTF8(text);
				_sgfOnFetchResponse(promise_ptr, response.status, text_cstr, 0, 0);
				_free(text_cstr);
			});
		}else{
			const ex_cstr = allocateUTF8(String(ex));
			_sgfOnFetchResponse(promise_ptr, -1, 0, 0, 0);
			_free(ex_cstr);
		}
	}).catch(ex => {
		const ex_cstr = allocateUTF8(String(ex));
		_sgfOnFetchResponse(promise_ptr, -1, 0, 0, ex_cstr);
		_free(ex_cstr);
	});
});
// clang-format on

} // namespace

Promise<FetchResponse> fetch(CString url) {
	auto promise_ptr = new Promise<FetchResponse>;
	sgfFetch(promise_ptr, url.c_str());
	return *promise_ptr;
}

} // namespace sgf

#pragma clang diagnostic pop
