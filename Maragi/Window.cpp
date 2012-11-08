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

			void setParent(const ControlWeakPtr<> &parent)
			{
				self->parent_ = parent;
			}

			ControlWeakPtr<> getChild()
			{
				return self->child_;
			}
		};

		Slot::Slot()
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent, &Impl::setParent);
			child.init(impl.get(), &Impl::getChild);
		}

		Slot::Slot(const ControlWeakPtr<> &iparent)
			: parent_(iparent)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent, &Impl::setParent);
			child.init(impl.get(), &Impl::getChild);
		}

		Slot::~Slot()
		{
		}

		bool Slot::attach(const ControlWeakPtr<> &child)
		{
			ControlPtr<> lchild = child.lock();
			if(!child_.lock() && lchild && !lchild->parent_)
			{
				child_ = child;
				lchild->parent_ = this;
				return true;
			}
			return false;
		}

		ControlWeakPtr<> Slot::detach()
		{
			if(child_.lock())
			{
				ControlWeakPtr<> child = child_;
				child_ = nullptr;
				return child;
			}
			return nullptr;
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

			Slot *getParent()
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
				self->resize(rect);
			}
		};

		Control::Control(const ControlID &iid)
			: parent_(nullptr)
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

		bool Control::fireEvent(const std::shared_ptr<ERDelegate<void (ControlEventArg &)>> &handlers, ControlEventArg &e)
		{
			;

			return true;
		}

		ControlPtr<> Control::sharedFromThis()
		{
			return ControlPtr<>(shared_from_this());
		}

		void Control::createDrawingResources(Drawing::Context &)
		{
		}

		void Control::discardDrawingResources(Drawing::Context &)
		{
		}

		void Control::resize(const Objects::RectangleF &)
		{
		}

		void Control::click(const Objects::PointF &)
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

		ShellPtr<> Shell::sharedFromThis()
		{
			return ShellPtr<>(shared_from_this());
		}
	}
}
