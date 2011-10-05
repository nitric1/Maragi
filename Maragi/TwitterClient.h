// $Id$

#pragma once

#include "URI.h"

namespace Maragi
{
	class TwitterClient
	{
	private:
		struct CurlWriteCallbackData
		{
			TwitterClient *client;
			std::vector<uint8_t> data;
		};

	private:
		CURL *curl;
		CurlWriteCallbackData cbd;
		std::string accessToken, accessTokenSecret;

	public:
		TwitterClient();
		~TwitterClient();

	private:
		static bool initializeCurl();
		static size_t curlWriteCallback(void *, size_t, size_t, void *);

	public:
		void authorize();

	private:
		bool sendRequest(const URI &uri);
	};
}
