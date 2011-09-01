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

	private:
		CURL *curl;
		CurlWriteCallbackData cbd;

	public:
		TwitterClient();
		~TwitterClient();

	private:
		static bool initializeCurl();
		static size_t curlWriteCallback(void *, size_t, size_t, void *);
	};
}
