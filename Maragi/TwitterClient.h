// $Id: TwitterClient.h 42 2012-08-04 18:23:44Z wdlee91 $

#pragma once

#include "URI.h"
#include "TwitterClientError.h"

namespace Maragi
{
	class TwitterClient
	{
	private:
		struct CurlWriteCallbackData
		{
			TwitterClient *client;
			std::vector<uint8_t> data;
			std::function<void (size_t)> cb;
		};

	private:
		CURL *curl;
		CurlWriteCallbackData cbd;
		std::string screenName, accessToken, accessTokenSecret;

	public:
		TwitterClient();
		TwitterClient(const std::string &, const std::string &, const std::string &);
		TwitterClient(const TwitterClient &);
		~TwitterClient();

	private:
		static bool initializeCurl();
		static size_t curlWriteCallback(void *, size_t, size_t, void *);

	public:
		bool authorize();
		const std::string &getScreenName();
		const std::string &getAccessToken();
		const std::string &getAccessTokenSecret();

	private:
		bool sendRequest(const URI &uri);
	};
}
