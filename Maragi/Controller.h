// $Id$

#pragma once

namespace Maragi
{
	template<typename EventArg>
	class Controller
	{
	private:
		std::multimap<std::wstring, std::function<bool(EventArg)>> eventMap;

	public:
		bool addEvent(const std::wstring &eventName, std::function<bool(EventArg)> fn)
		{
			return eventMap.insert(make_pair(eventName, fn)).second;
		}

		bool removeEvent(const std::wstring &eventName, std::function<bool(EventArg)> fn);

		bool callEvent(const std::wstring &eventName, const EventArg &arg)
		{
			auto range = eventMap.equal_range(eventName);
			for(auto it = range.first; it != range.second; ++ it)
			{
				if(!it->second(arg))
					return false;
			}

			return true;
		}
	};
}
