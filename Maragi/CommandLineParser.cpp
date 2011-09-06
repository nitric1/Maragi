// $Id$

#include "Defaults.h"

#include "CommandLineParser.h"

namespace Maragi
{
	CommandLineParser::CommandLineParser()
	{
	}

	CommandLineParser::CommandLineParser(const CommandLineParser &obj)
		: abbrs(obj.abbrs), args(obj.args)
	{
	}

	CommandLineParser::CommandLineParser(CommandLineParser &&obj)
		: abbrs(std::move(obj.abbrs)), args(std::move(obj.args))
	{
	}

	CommandLineParser::~CommandLineParser()
	{
	}

	bool CommandLineParser::addAbbr(const std::wstring &abbr, const std::wstring &full)
	{
		return abbrs.insert(make_pair(abbr, full)).second;
	}

	void CommandLineParser::removeAbbr(const std::wstring &abbr)
	{
		abbrs.erase(abbr);
	}

	void CommandLineParser::parse(const std::wstring &commandLine)
	{
		std::vector<std::wstring> words = scan(commandLine);

		enum
		{
			NONE,
			IN_WORD,
			AFTER_DASH,
			IN_ABBR,
			IN_FULL,
		} wordToken = NONE;

		auto it = words.begin(), ed = words.end();
		for(; it != ed; ++ it)
		{
			switch(wordToken)
			{
			case NONE:
				break;
			}
		}
	}

	std::vector<std::wstring> CommandLineParser::scan(const std::wstring &commandLine)
	{
		std::vector<std::wstring> words;

		enum
		{
			NEW_WORD,
			IN_WORD,
			IN_QUOTATION,
			AFTER_QUOTATION,
		} charToken = NEW_WORD;
		std::wstring str;

		auto it = commandLine.begin(), ed = commandLine.end();
		for(; it != ed; ++ it)
		{
			switch(charToken)
			{
			case NEW_WORD:
				if(*it == L'"')
					charToken = IN_QUOTATION;
				else if(*it != ' ')
				{
					str += *it;
					charToken = IN_WORD;
				}
				break;

			case IN_WORD:
				if(*it == L'"')
					charToken = IN_QUOTATION;
				else if(*it != ' ')
					str += *it;
				else
				{
					words.push_back(str);
					str.clear();
					charToken = NEW_WORD;
				}
				break;

			case IN_QUOTATION:
				if(*it == L'"')
					charToken = AFTER_QUOTATION;
				else
					str += *it;
				break;

			case AFTER_QUOTATION:
				if(*it == L' ')
				{
					words.push_back(str);
					str.clear();
					charToken = NEW_WORD;
				}
				else if(*it == L'"')
				{
					str += L'"';
					charToken = IN_QUOTATION;
				}
				else
				{
					str += *it;
					charToken = IN_WORD;
				}
				break;
			}
		}

		if(charToken != NEW_WORD)
		{
			words.push_back(str);
		}

		return words;
	}
}
