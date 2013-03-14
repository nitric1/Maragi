// $Id: TwitterClientError.h 42 2012-08-04 18:23:44Z wdlee91 $

#pragma once

namespace Maragi
{
	class TwitterClientError : public std::runtime_error
	{
	public:
		explicit TwitterClientError(const char *) throw();
		explicit TwitterClientError(const std::string &) throw();
		TwitterClientError(const TwitterClientError &) throw();
		virtual ~TwitterClientError() throw();
	};

	class NotAuthorizedError : public TwitterClientError
	{
	public:
		explicit NotAuthorizedError(const char *) throw();
		explicit NotAuthorizedError(const std::string &) throw();
		NotAuthorizedError(const NotAuthorizedError &) throw();
		virtual ~NotAuthorizedError() throw();
	};

	class AccessDeniedError: public TwitterClientError
	{
	public:
		explicit AccessDeniedError(const char *) throw();
		explicit AccessDeniedError(const std::string &) throw();
		AccessDeniedError(const AccessDeniedError &) throw();
		virtual ~AccessDeniedError() throw();
	};

	class NotFoundError: public TwitterClientError
	{
	public:
		explicit NotFoundError(const char *) throw();
		explicit NotFoundError(const std::string &) throw();
		NotFoundError(const NotFoundError &) throw();
		virtual ~NotFoundError() throw();
	};

	class ConnectionLostError: public TwitterClientError
	{
	public:
		explicit ConnectionLostError(const char *) throw();
		explicit ConnectionLostError(const std::string &) throw();
		ConnectionLostError(const ConnectionLostError &) throw();
		virtual ~ConnectionLostError() throw();
	};

	class ResponseNotParsableError: public TwitterClientError
	{
	public:
		explicit ResponseNotParsableError(const char *) throw();
		explicit ResponseNotParsableError(const std::string &) throw();
		ResponseNotParsableError(const ResponseNotParsableError &) throw();
		virtual ~ResponseNotParsableError() throw();
	};
}