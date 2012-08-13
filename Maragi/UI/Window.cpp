// $Id$

#include "../Common.h"

#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		WindowID WindowID::undefined(static_cast<uintptr_t>(-1));

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
				RECT rc;
				GetWindowRect(self->id, &rc);
			}

			void setRect(const Objects::Rectangle &rc)
			{
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

		uintptr_t VirtualControl::newId = 0;

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
				return static_cast<Shell *>(static_cast<Window *>(self->Window::parent));
			}
		};

		Shell::Shell() // no parent
			: Window(nullptr, WindowID::undefined)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
		}

		Shell::Shell(Shell *iparent) // with parent
			: Window(iparent, WindowID::undefined)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
		}
	}
}
