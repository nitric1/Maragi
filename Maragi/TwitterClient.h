// $Id$

#pragma once

namespace Maragi
{
	class TwitterClient
	{
	private:
		struct CurlWriteCallbackData
		{
			TwitterClient *client;
			std::vector<char> data;
		};

	public:
		enum
		{

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
		;
	};
}
