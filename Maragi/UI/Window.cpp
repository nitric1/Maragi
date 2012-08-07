// $Id$

#include "../Common.h"

#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		class Window::Impl
		{
		private:
			Window *self;

		public:
			Impl(Window *iself)
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

			int32_t getX()
			{
			}

			void setX(int32_t x)
			{
			}

			int32_t getY()
			{
			}

			void setY(int32_t y)
			{
			}

			int32_t getWidth()
			{
			}

			void setWidth(int32_t width)
			{
			}

			int32_t getHeight()
			{
			}

			void setHeight(int32_t height)
			{
			}
		};

		Window::Window(Window *iparent, WindowID iid)
			: _parent(iparent)
			, _id(iid)
		{
			impl = std::shared_ptr<Impl *>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
			id.init(impl.get(), &Impl::getId);
			x.init(impl.get(), &Impl::getX, &Impl::setX);
			y.init(impl.get(), &Impl::getY, &Impl::setY);
			width.init(impl.get(), &Impl::getWidth, &Impl::setWidth);
			height.init(impl.get(), &Impl::getHeight, &Impl::setHeight);
		}

		Window::~Window()
		{
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

		uintptr_t VirtualControl::newId = 0;

		Shell::Shell(Shell *parent)
			: Window(parent)
		{
		}

		Shell *Shell::getParent()
		{
			return static_cast<Shell *>(parent);
		}

		const Shell *Shell::getParent() const
		{
			return static_cast<const Shell *>(parent);
		}
	}
}
