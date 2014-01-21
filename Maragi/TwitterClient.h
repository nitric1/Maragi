#pragma once

#include "Batang/Thread.h"

#include "Url.h"
#include "TwitterClientError.h"

namespace Maragi
{
    class TwitterClient : public Batang::Thread<TwitterClient>
    {
    private:
        struct CurlWriteCallbackData
        {
            TwitterClient *client;
            std::vector<uint8_t> data;
            std::function<void (size_t)> cb;
        };

    private:
        CURL *curl_;
        CurlWriteCallbackData cbd_;
        std::string screenName_, accessToken_, accessTokenSecret_;

    public:
        TwitterClient();
        TwitterClient(const std::string &, const std::string &, const std::string &);
        TwitterClient(const TwitterClient &);
        ~TwitterClient();

    private:
        static bool initializeCurl();
        static size_t curlWriteCallback(void *, size_t, size_t, void *);

    public:
        void authorize();
        const std::string &screenName();
        const std::string &accessToken();
        const std::string &accessTokenSecret();

    private:
        bool sendRequest(const Url &uri);
    };
}
