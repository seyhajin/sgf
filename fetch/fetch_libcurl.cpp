#include "fetch_libcurl.h"

#include <curl/curl.h>
#include <thread>

namespace sgf {

namespace {

thread_local CURL* g_curl;

// Note: can't use a lambda for this for some reason.
size_t writeFunc(void* contents, size_t size, size_t nmemb, void* data) {
	auto text = (String*)data;
	size *= nmemb;
	text->append((char*)contents, size);
	return size;
}

} // namespace

Promise<FetchResponse> fetch(CString url) {

	Promise<FetchResponse> promise;

	std::thread([promise, url]() mutable {
		FetchResponse response{-1};

		if (!g_curl) {
			g_curl = curl_easy_init();
			if (!g_curl) {
				debug("!!! Failed to create CURL instance");
				promise.resolve(response);
				return;
			}

			curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, &writeFunc);
//			curl_easy_setopt(g_curl, CURLOPT_BUFFERSIZE, 512 * 1024);
			curl_easy_setopt(g_curl, CURLOPT_ACCEPT_ENCODING, ""); // Important! Should be the default!
			curl_easy_setopt(g_curl, CURLOPT_HTTP_CONTENT_DECODING, 0);

			// curl_easy_setopt(g_curl,CURLOPT_VERBOSE,1 );

			curl_slist* slist{};
#if 0
			// Some more opt to try next time yer stuck!
			curl_easy_setopt(g_curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
			curl_easy_setopt(g_curl, CURLOPT_BUFFERSIZE, 512 * 1024);
			curl_easy_setopt(g_curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv2);
			curl_easy_setopt(g_curl, CURLOPT_BUFFERSIZE, 512 * 1024);
			curl_easy_setopt(g_curl, CURLOPT_FOLLOWLOCATION, 1);
			curl_easy_setopt(g_curl, CURLOPT_AUTOREFERER, 1 );
			curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
			curl_easy_setopt(g_curl, CURLOPT_DEBUGFUNCTION, &debugFunc);

			slist = curl_slist_append(slist, "Cache-Control: no-cache");
			slist = curl_slist_append(slist, "pragma: no-cache");
			slist = curl_slist_append(slist, "Connection: keep-alive");
			slist = curl_slist_append(slist, "Host: roa.nz");
			slist = curl_slist_append(slist, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:105.0) Gecko/20100101 Firefox/105.0");
			slist = curl_slist_append(slist, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
			slist = curl_slist_append(slist, "Accept-Language: en-US,en;q=0.5");
			slist = curl_slist_append(slist, "Accept-Encoding: gzip, deflate, br");
			slist = curl_slist_append(slist, "Connection: keep-alive");
			slist = curl_slist_append(slist, "Upgrade-Insecure-Requests: 1");
			slist = curl_slist_append(slist, "Sec-Fetch-Dest: document");
			slist = curl_slist_append(slist, "Sec-Fetch-Mode: navigate");
			slist = curl_slist_append(slist, "Sec-Fetch-Site: none");
			slist = curl_slist_append(slist, "Sec-Fetch-User: ?1");
#endif
			if(slist) curl_easy_setopt(g_curl, CURLOPT_HTTPHEADER, slist);
		}

		// Fudged for orkland texture tiles.
		response.text.reserve(0x110000);

		char error[CURL_ERROR_SIZE] = "";

		curl_easy_setopt(g_curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(g_curl, CURLOPT_WRITEDATA, &response.text);
		curl_easy_setopt(g_curl, CURLOPT_ERRORBUFFER, error);

		curl_easy_perform(g_curl);
		curl_easy_getinfo(g_curl, CURLINFO_RESPONSE_CODE, &response.httpStatus);

		response.error = error;

#if DEBUG
		if (response.httpStatus < 200 || response.httpStatus >= 300) {
//			debug() << "!!! HTTP fetch failed" << url << response.httpStatus;
		}
#endif
		// curl_easy_cleanup(g_curl);

		promise.resolveAsync(response);

	}).detach();

	return promise;
}

} // namespace sgf
