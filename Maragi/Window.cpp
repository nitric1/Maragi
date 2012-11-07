// $Id$

#include "Common.h"

#include "Global.h"
#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		const ControlID ControlID::undefined(0);

		class Slot::Impl
		{
		private:
			Slot *self;

		public:
			explicit Impl(Slot *iself)
				: self(iself)
			{}

			ControlWeakPtr<> getParent()
			{
				return self->parent_;
			}

			ControlWeakPtr<> getChild()
			{
				return self->child_;
			}
		};

		Slot::Slot(const ControlWeakPtr<> &iparent)
			: parent_(iparent)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
			child.init(impl.get(), &Impl::getChild);
		}

		Slot::~Slot()
		{
		}

		bool Slot::attach(const ControlWeakPtr<> &child)
		{
			if(!child_.lock())
			{
				child_ = child;
				return true;
			}
			return false;
		}

		bool Slot::detach()
		{
			if(child_.lock())
			{
				child_ = nullptr;
				return true;
			}
			return false;
		}

		class Control::Impl
		{
		private:
			Control *self;

		public:
			explicit Impl(Control *iself)
				: self(iself)
			{
			}

			ControlWeakPtr<> getParent()
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

		Control::Control(Slot *slot, const ControlID &iid)
			: parent_(slot ? slot->parent : nullptr)
			, id_(iid)
		{
			if(slot)
			{
				// TODO: slot attach
			}

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

		void Control::createDrawingResources(Drawing::Context &)
		{
		}

		void Control::discardDrawingResources(Drawing::Context &)
		{
		}

		void Control::draw(Drawing::Context &context)
		{
		}

		class Shell::Impl
		{
		private:
			Shell *self;

		public:
			explicit Impl(Shell *iself)
				: self(iself)
			{}

			ShellWeakPtr<> getParent()
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

		Shell::Shell(const ShellWeakPtr<> &iparent) // with parent
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
