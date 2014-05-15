#pragma once

#include "Batang/Event.h"
#include "Batang/Thread.h"

#include "Url.h"
#include "TwitterClientError.h"

namespace Maragi
{
    class TwitterClient : public Batang::Thread<TwitterClient>
    {
    public:
        struct TwitterEventArg
        {
            std::wstring authorId_;
            std::wstring authorName_;
            std::wstring text_;
        };

    private:
        typedef Batang::Event<TwitterEventArg> TwitterEvent;
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
        TwitterClient(const TwitterClient &) = delete;
        ~TwitterClient();

    private:
        static size_t curlWriteCallback(void *, size_t, size_t, void *);

    public:
        void authorize();
        bool authorized() const;
        const std::string &screenName() const;
        const std::string &accessToken() const;
        const std::string &accessTokenSecret() const;

    public:
        TwitterEvent onStreamAdd;

    private:
        void run();

    private:
        bool sendRequest(const Url &uri);

        friend class Batang::Thread<TwitterClient>;
    };
}
