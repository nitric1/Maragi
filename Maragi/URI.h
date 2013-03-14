// $Id: URI.h 42 2012-08-04 18:23:44Z wdlee91 $

#pragma once

namespace Maragi
{
	class URI
	{
	private:
		std::string baseURI;
		std::map<std::string, std::string> params;
		std::map<std::string, std::string> oauthParams;
		mutable bool changed;
		mutable std::string uriString;

	public:
		URI();
		URI(
			const std::string &,
			const std::map<std::string, std::string> & = std::map<std::string, std::string>(),
			const std::map<std::string, std::string> & = std::map<std::string, std::string>()
		);
		URI(const URI &);
		URI(URI &&);

	public:
		void assign(
			const std::string &,
			const std::map<std::string, std::string> & = std::map<std::string, std::string>(),
			const std::map<std::string, std::string> & = std::map<std::string, std::string>()
		);
		void assignBaseURI(const std::string &);
		void assignParam(const std::map<std::string, std::string> &);
		void assignOAuthParam(const std::map<std::string, std::string> &);
		bool hasParam(const std::string &) const;
		const std::string &getParam(const std::string &) const;
		bool addParam(const std::string &, const std::string &);
		void removeParam(const std::string &);
		bool hasOAuthParam(const std::string &) const;
		const std::string &getOAuthParam(const std::string &) const;
		bool addOAuthParam(const std::string &, const std::string &);
		void removeOAuthParam(const std::string &);
		const std::string &getBaseURI() const;
		const std::map<std::string, std::string> &getParams() const;
		const std::map<std::string, std::string> &getOAuthParams() const;
		std::string getStringURI() const;

	public:
		URI &operator =(const URI &);
		URI &operator =(URI &&);
		operator std::string() const;
	};
}
