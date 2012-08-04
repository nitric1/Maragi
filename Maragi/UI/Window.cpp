// $Id$

#include "../Common.h"

#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		Window::Window(Window *iparent)
			: parent(iparent)
		{
		}

		Window::~Window()
		{
		}

		Window *Window::getParent()
		{
			return parent;
		}

		const Window *Window::getParent() const
		{
			return parent;
		}

		WindowID Window::getID() const
		{
			return id;
		}

		void Window::release()
		{
			delete this;
		}

		bool Window::addEventListener(const std::wstring &name, std::shared_ptr<ERDelegate<bool (WindowEventArg)>> listener)
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
				if(!(*it->second)(e))
					return false;
			}

			return true;
		}

		HWNDWindow::HWNDWindow()
			: handle(nullptr)
		{
		}

		HWND HWNDWindow::getHandle() const
		{
			return handle;
		}

		void HWNDWindow::setHandle(HWND ihandle)
		{
			handle = ihandle;
		}

		Shell::Shell(Shell *parent)
			: Window(parent)
		{
		}

		Shell *Shell::getParent()
		{
			return static_cast<Shell *>(Window::getParent());
		}

		const Shell *Shell::getParent() const
		{
			return static_cast<const Shell *>(Window::getParent());
		}
	}
}
