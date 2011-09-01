// $Id$

#pragma once

namespace Maragi
{
	struct WindowEventArgs
	{
		HWND window;
		void *param1; // TODO: Change required
		void *param2;
	};

	template<typename EventArg = WindowEventArgs>
	class Window
	{
	private:
		std::multimap<std::wstring, std::function<bool(EventArg)>> eventMap;

	public:
		virtual ~Window() {}

	public:
		bool addEventListener(const std::wstring &eventName, std::function<bool(EventArg)> fn)
		{
			return eventMap.insert(make_pair(eventName, fn)).second;
		}

		bool fireEvent(const std::wstring &eventName, const EventArg &arg)
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
