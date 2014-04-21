#pragma once

namespace Maragi
{
    class TwitterClientError : public std::runtime_error
    {
    public:
        explicit TwitterClientError(const char *) noexcept;
        explicit TwitterClientError(const std::string &) noexcept;
        TwitterClientError(const TwitterClientError &) noexcept;
        virtual ~TwitterClientError() noexcept;
    };

    class NotAuthorizedError : public TwitterClientError
    {
    public:
        explicit NotAuthorizedError(const char *) noexcept;
        explicit NotAuthorizedError(const std::string &) noexcept;
        NotAuthorizedError(const NotAuthorizedError &) noexcept;
        virtual ~NotAuthorizedError() noexcept;
    };

    class AccessDeniedError: public TwitterClientError
    {
    public:
        explicit AccessDeniedError(const char *) noexcept;
        explicit AccessDeniedError(const std::string &) noexcept;
        AccessDeniedError(const AccessDeniedError &) noexcept;
        virtual ~AccessDeniedError() noexcept;
    };

    class NotFoundError: public TwitterClientError
    {
    public:
        explicit NotFoundError(const char *) noexcept;
        explicit NotFoundError(const std::string &) noexcept;
        NotFoundError(const NotFoundError &) noexcept;
        virtual ~NotFoundError() noexcept;
    };

    class ConnectionLostError: public TwitterClientError
    {
    public:
        explicit ConnectionLostError(const char *) noexcept;
        explicit ConnectionLostError(const std::string &) noexcept;
        ConnectionLostError(const ConnectionLostError &) noexcept;
        virtual ~ConnectionLostError() noexcept;
    };

    class ResponseNotParsableError: public TwitterClientError
    {
    public:
        explicit ResponseNotParsableError(const char *) noexcept;
        explicit ResponseNotParsableError(const std::string &) noexcept;
        ResponseNotParsableError(const ResponseNotParsableError &) noexcept;
        virtual ~ResponseNotParsableError() noexcept;
    };
}
