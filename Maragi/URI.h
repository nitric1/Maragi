#pragma once

namespace Maragi
{
	class URI
	{
	private:
		std::string baseURI;
		std::map<std::string, std::string> params;
		mutable bool changed;
		mutable std::string uriString;

	public:
		URI();
		URI(const std::string &);
		URI(const std::string &, const std::map<std::string, std::string> &);
		URI(const URI &);
		URI(URI &&);

	public:
		void assign(const std::string &);
		void assign(const std::string &, const std::map<std::string, std::string> &);
		void assign(const std::map<std::string, std::string> &);
		bool addParam(const std::string &, const std::string &);
		void removeParam(const std::string &);
		const std::string &getBaseURI() const;
		const std::map<std::string, std::string> &getParams() const;
		std::string getStringURI() const;

	public:
		URI &operator =(const URI &);
		URI &operator =(URI &&);
		operator std::string() const;
	};
}
