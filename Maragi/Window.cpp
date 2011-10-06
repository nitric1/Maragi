// $Id$

#include "Common.h"

#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		Window::Window(HWND iwindow)
			: window(iwindow)
		{
		}
		
		Window::~Window()
		{
		}

		int Window::getShowStatus() const
		{
			return showCommand;
		}

		void Window::setShowStatus(int ishowCommand)
		{
			showCommand = ishowCommand;
		}

		HWND Window::getWindow() const
		{
			return window;
		}

		void Window::setWindow(HWND iwindow)
		{
			window = iwindow;
		}

		Window *Window::getParent()
		{
			return parent;
		}

		const Window *Window::getParent() const
		{
			return parent;
		}

		void Window::setParent(Window *iparent)
		{
			parent = iparent;
		}

		bool Window::showWithParent(Window *parent)
		{
			setParent(parent);
			return show();
		}

		bool Window::addEventListener(const std::wstring &name, std::shared_ptr<WindowEventCallback> listener)
		{
			eventMap.insert(make_pair(name, listener));
			return true;
		}

		bool Window::fireEvent(const std::wstring &name, WindowEventArg e)
		{
			if(eventMap.find(name) == eventMap.end())
				return true;

			auto r = eventMap.equal_range(name);
			for(auto it = r.first; it != r.second; ++ it)
			{
				if(!it->second->run(e))
					return false;
			}

			return true;
		}
	}
}
