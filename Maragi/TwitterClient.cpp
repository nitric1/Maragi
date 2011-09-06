// $Id$

#include "Defaults.h"

#include "TwitterClient.h"

#include "Constants.h"

namespace Maragi
{
	TwitterClient::TwitterClient()
	{
		initializeCurl();

		curl = curl_easy_init();
		if(curl == nullptr)
			throw(std::runtime_error("CURL initialization failed."));

		std::vector<char> agent;
		agent.resize(static_cast<size_t>(WideCharToMultiByte(CP_UTF8, 0, Constants::userAgent, -1, nullptr, 0, nullptr, nullptr)) + 1);
		WideCharToMultiByte(CP_UTF8, 0, Constants::userAgent, -1, &*agent.begin(), static_cast<int>(agent.size()), nullptr, nullptr);

		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1l);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, &*agent.begin());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlWriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void *>(&cbd));

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0l);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0l);
	}

	TwitterClient::~TwitterClient()
	{
		curl_easy_cleanup(curl);
	}

	bool TwitterClient::initializeCurl()
	{
		static bool initialized = false;

		if(!initialized && curl_global_init(CURL_GLOBAL_ALL) == 0)
			initialized = true;

		return initialized;
	}

	size_t TwitterClient::curlWriteCallback(void *data, size_t size, size_t nmemb, void *param)
	{
		CurlWriteCallbackData *cbd = static_cast<CurlWriteCallbackData *>(param);
		size_t realSize = size * nmemb;

		cbd->data.insert(cbd->data.end(), static_cast<char *>(data), static_cast<char *>(data) + realSize);

		// TODO: Call callback function

		return realSize;
	}
}
