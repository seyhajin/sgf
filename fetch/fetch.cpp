#include "fetch.h"

#include <curl/curl.h>

namespace sgf {

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

		FetchResponse response{-1,url};

		CURL* curl = curl_easy_init();
		if (!curl) {
			promise.resolve(response);
			return;
		}

		char error[CURL_ERROR_SIZE] = "";

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeFunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.data);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

		curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.httpStatus);
		response.error = error;
		curl_easy_cleanup(curl);

		promise.resolve(response);
	};

	Thread thread(threadFunc);
	thread.detach();

	return promise;
}

} // namespace sgf
