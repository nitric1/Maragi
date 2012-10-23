// $Id$

#include "../Common.h"

#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		const WindowID WindowID::undefined(0);

		class Window::Impl
		{
		private:
			Window *self;

		public:
			explicit Impl(Window *iself)
				: self(iself)
			{
			}

			Window *getParent()
			{
				return self->_parent;
			}

			WindowID getId()
			{
				return self->_id;
			}

			void setId(const WindowID &id)
			{
				self->_id = id;
			}

			Objects::Rectangle getRect()
			{
				return self->_rect;
			}

			void setRect(const Objects::Rectangle &rect)
			{
				self->_rect = rect;
			}
		};

		Window::Window(Window *iparent, WindowID iid)
			: _parent(iparent)
			, _id(iid)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
			id.init(impl.get(), &Impl::getId, &Impl::setId);
			rect.init(impl.get(), &Impl::getRect, &Impl::setRect);
		}

		Window::~Window()
		{
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

		WindowManager::WindowManager()
		{
		}

		WindowManager::~WindowManager()
		{
		}

		WindowID WindowManager::getNextID()
		{
			++ nextID.id;
			return nextID;
		}

		void WindowManager::add(Window *window)
		{
			windows.insert(std::make_pair(static_cast<WindowID>(window->id), window));
		}

		Window *WindowManager::find(WindowID id)
		{
			auto it = windows.find(id);
			if(it == windows.end())
				return nullptr;
			return it->second;
		}

		void WindowManager::remove(WindowID id)
		{
			windows.erase(id);
		}

		/*
		class Control::Impl
		{
		private:
			Control *self;

		public:
			explicit Impl(Control *iself)
				: self(iself)
			{
			}
		};

		Control::Control()
			: Window(nullptr, WindowID::undefined)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
		}*/

		class Shell::Impl
		{
		private:
			Shell *self;

		public:
			explicit Impl(Shell *iself)
				: self(iself)
			{
			}

			Shell *getParent()
			{
				return self->_parent;
			}

			HWND getHwnd()
			{
				return self->_hwnd;
			}

			void setHwnd(HWND hwnd)
			{
				self->_hwnd = hwnd;
			}
		};

		Shell::Shell() // no parent
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
			hwnd.init(impl.get(), &Impl::getHwnd, &Impl::setHwnd);
		}

		Shell::Shell(Shell *iparent) // with parent
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
			hwnd.init(impl.get(), &Impl::getHwnd, &Impl::setHwnd);
		}
	}
}
