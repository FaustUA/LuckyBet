#pragma once

#include "curl/curl.h"

#ifdef _DEBUG
#pragma comment(lib, "src/curl/libcurl_a_debug.lib")
#else
#pragma comment(lib, "src/curl/libcurl_a.lib")
#endif

static int curl_writer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
	if (writerData == NULL)
		return 0;

	writerData->append(data, size*nmemb);

	return size * nmemb;
}
