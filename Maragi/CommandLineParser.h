// $Id$

#pragma once

namespace Maragi
{
	class CommandLineParser
	{
	private:
		std::map<std::wstring, std::wstring> abbrs;
		std::multimap<std::wstring, std::wstring> args;

	public:
		CommandLineParser();
		CommandLineParser(const CommandLineParser &);
		CommandLineParser(CommandLineParser &&);
		virtual ~CommandLineParser();

	public:
		bool addAbbr(const std::wstring &, const std::wstring &);
		void removeAbbr(const std::wstring &);
		void parse(const std::wstring &);

	private:
		std::vector<std::wstring> scan(const std::wstring &);
	};
}
