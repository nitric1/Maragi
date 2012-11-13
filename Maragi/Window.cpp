// $Id$

#include "Common.h"

#include "Global.h"
#include "Layouts.h"
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

		bool Slot::attach(const ControlWeakPtr<> &newChild)
		{
			ControlPtr<> lnewChild = newChild.lock();
			if(!child_.lock() && lnewChild && !lnewChild->parent_)
			{
				child_ = newChild;
				lnewChild->parent_ = this;
				return true;
			}
			return false;
		}

		ControlWeakPtr<> Slot::detach()
		{
			ControlPtr<> lchild = child_.lock();
			if(lchild)
			{
				lchild->parent_ = nullptr;
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

			ShellWeakPtr<> getShell()
			{
				ControlWeakPtr<> ctl = self->sharedFromThis();
				ControlPtr<> lctl;
				while(true)
				{
					lctl = ctl.lock();
					if(!lctl || !lctl->parent_)
						break;
					ctl = lctl->parent_->parent;
				}
				ControlPtr<ShellLayout> llayout = lctl;
				if(llayout)
					return llayout->shell;
				return nullptr;
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
				self->onResizeInternal(rect);
			}
		};

		Control::Control(const ControlID &iid)
			: parent_(nullptr)
			, id_(iid)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			shell.init(impl.get(), &Impl::getShell);
			parent.init(impl.get(), &Impl::getParent);
			id.init(impl.get(), &Impl::getId);
			rect.init(impl.get(), &Impl::getRect, &Impl::setRect);
		}

		Control::~Control()
		{
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

		ControlWeakPtr<> Control::findByPoint(const Objects::PointF &pt)
		{
			if(rect_.isIn(pt))
				return sharedFromThis();
			return nullptr;
		}

		std::vector<ControlWeakPtr<>> Control::findTreeByPoint(const Objects::PointF &pt)
		{
			if(rect_.isIn(pt))
				return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
			return std::vector<ControlWeakPtr<>>();
		}

		std::vector<ControlWeakPtr<>> Control::findReverseTreeByPoint(const Objects::PointF &pt)
		{
			if(rect_.isIn(pt))
				return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
			return std::vector<ControlWeakPtr<>>();
		}

		void Control::walk(const std::function<void (const ControlWeakPtr<> &)> &fn)
		{
			fn(sharedFromThis());
		}

		void Control::walkReverse(const std::function<void (const ControlWeakPtr<> &)> &fn)
		{
			fn(sharedFromThis());
		}

		void Control::redraw()
		{
			ShellPtr<> lshell = shell.get().lock();
			if(lshell)
				lshell->redraw();
		}

		void Control::onResizeInternal(const Objects::RectangleF &)
		{
		}

		bool Control::onSetCursorInternal()
		{
			return false;
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

		void Shell::redraw()
		{
		}
	}
}
