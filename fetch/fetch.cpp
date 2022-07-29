#include "fetch.h"

#ifdef OS_EMSCRIPTEN
#include <emscripten.h>
#else
#include <curl/curl.h>
#include <thread>
#endif

namespace sgf {

#ifdef OS_EMSCRIPTEN

namespace {

extern "C" {

EMSCRIPTEN_KEEPALIVE void sgfOnFetchResponse(Promise<FetchResponse>* promise_ptr, int status, const char* text_cstr, const char* error_cstr) {
	promise_ptr->resolve({status, text_cstr ? String(text_cstr) : String(),error_cstr ? String(error_cstr) : String ()});
	delete promise_ptr;
}

// clang-format off
EM_JS(void,sgfFetch,(Promise<FetchResponse>* promise_ptr, const char* url_cstr),{

	const url = UTF8ToString(url_cstr);

	fetch(url).then(response => {
		response.text().then(text => {
			const text_cstr = allocateUTF8(text);
			_sgfOnFetchResponse(promise_ptr, response.status, text_cstr, 0);
			_free(text_cstr);
		});
	}).catch(ex => {
		const ex_cstr = allocateUTF8(String(ex));
		_sgfOnFetchResponse(promise_ptr, -1, 0, ex_cstr);
		_free(ex_cstr);
	});
});
// clang-format on
}

}

Promise<FetchResponse> fetch(CString url) {
	auto promise_ptr = new Promise<FetchResponse>;
	sgfFetch(promise_ptr, url.c_str());
	return *promise_ptr;
}

#else

namespace {

// Note: can't use a lambda for this for some reason.
size_t writeFunc(void *contents, size_t size, size_t nmemb, void *data) {
	((String*)data)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

}

Promise<FetchResponse> fetch(CString url) {

	Promise<FetchResponse> promise;

	auto threadFunc = [promise, url]() mutable {

		FetchResponse response{-1};

		CURL* curl = curl_easy_init();
		if (!curl) {
			promise.resolve(response);
			return;
		}

		char error[CURL_ERROR_SIZE] = "";

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeFunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.text);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

		curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.httpStatus);
		curl_easy_cleanup(curl);
		response.error=error;

		promise.resolve(response);
	};

	std::thread thread(threadFunc);
	thread.detach();

	return promise;
}

#endif

} // namespace sgf
