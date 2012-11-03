// $Id$

#include "Common.h"

#include "Global.h"
#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		const ControlID ControlID::undefined(0);

		class Control::Impl
		{
		private:
			Control *self;

		public:
			explicit Impl(Control *iself)
				: self(iself)
			{
			}

			ControlPtr<Control> getParent()
			{
				return self->parent_;
			}

			ControlID getId()
			{
				return self->id_;
			}

			Objects::RectangleF getRect()
			{
				return self->rect_;
			}

			void setRect(const Objects::RectangleF &rect)
			{
				self->rect_ = rect;
			}
		};

		Control::Control(const ControlPtr<Control> &iparent, const ControlID &iid)
			: parent_(iparent)
			, id_(iid)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
			id.init(impl.get(), &Impl::getId);
			rect.init(impl.get(), &Impl::getRect, &Impl::setRect);
		}

		Control::~Control()
		{
		}

		bool Control::addEventListener(const std::wstring &name, std::shared_ptr<ERDelegate<bool (ControlEventArg)>> listener)
		{
			eventMap.insert(make_pair(name, listener));
			return true;
		}

		bool Control::fireEvent(const std::wstring &name, ControlEventArg e)
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

		void Control::draw(Context &context)
		{
		}

		class Shell::Impl
		{
		private:
			Shell *self;

		public:
			explicit Impl(Shell *iself)
				: self(iself)
			{
			}

			ShellPtr<Shell> getParent()
			{
				return self->parent_;
			}

			HWND getHwnd()
			{
				return self->hwnd_;
			}

			Objects::SizeI getClientSize()
			{
				RECT rc;
				GetClientRect(self->hwnd_, &rc);
				return Objects::SizeI(rc.right - rc.left, rc.bottom - rc.top);
			}
		};

		Shell::Shell() // no parent
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
			hwnd.init(impl.get(), &Impl::getHwnd);
			clientSize.init(impl.get(), &Impl::getClientSize);
		}

		Shell::Shell(const ShellPtr<Shell> &iparent) // with parent
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
			hwnd.init(impl.get(), &Impl::getHwnd);
			clientSize.init(impl.get(), &Impl::getClientSize);
		}

		Shell::~Shell()
		{
		}
	}
}
