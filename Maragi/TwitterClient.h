// $Id$

#pragma once

#include "URI.h"

namespace Maragi
{
	class NotAuthorizedError : public std::runtime_error
	{
	public:
		explicit NotAuthorizedError(const char *) throw();
		explicit NotAuthorizedError(const std::string &) throw();
		NotAuthorizedError(const NotAuthorizedError &) throw();
		virtual ~NotAuthorizedError() throw();
	};

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
		bool saveAccessToken();

	private:
		bool sendRequest(const URI &uri);
	};
}
