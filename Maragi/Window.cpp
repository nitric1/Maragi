// $Id: Window.cpp 83 2012-11-21 06:12:41Z wdlee91 $

#include "Common.h"

#include "Global.h"
#include "Layouts.h"
#include "UIGlobal.h"
#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		const ControlID ControlID::undefined(0);

		Slot::Slot()
		{
		}

		Slot::Slot(const ControlWeakPtr<> &iparent)
			: parent_(iparent)
		{
		}

		Slot::~Slot()
		{
			detach();
		}

		bool Slot::attach(const ControlWeakPtr<> &newChild)
		{
			ControlPtr<> lnewChild = newChild.lock();
			if(!child_.lock() && lnewChild && !lnewChild->parent_)
			{
				child_ = newChild;
				lnewChild->parent_ = this;
				ControlPtr<> lparent = parent_.lock();
				if(lparent)
				{
					lnewChild->walk([lparent](const ControlWeakPtr<> &ctl)
					{
						ControlPtr<> lctl = ctl.lock();
						if(lctl)
							lctl->shell_ = lparent->shell_;
					});
				}
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
				lchild->walk([](const ControlWeakPtr<> &ctl)
				{
					ControlPtr<> lctl = ctl.lock();
					if(lctl)
						lctl->shell_ = nullptr;
				});
				ControlWeakPtr<> child = child_;
				child_ = nullptr;
				return child;
			}
			return nullptr;
		}

		const ControlWeakPtr<> &Slot::parent() const
		{
			return parent_;
		}

		void Slot::parent(const ControlWeakPtr<> &iparent)
		{
			parent_ = iparent;
		}

		const ControlWeakPtr<> &Slot::child() const
		{
			return child_;
		}

		Control::Control(const ControlID &iid)
			: parent_(nullptr)
			, id_(iid)
		{
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
			ShellPtr<> lshell = shell_.lock();
			if(lshell)
				lshell->redraw();
		}

		void Control::focus()
		{
			ShellPtr<> lshell = shell_.lock();
			if(lshell)
				lshell->focus(sharedFromThis());
		}

		void Control::blur()
		{
			ShellPtr<> lshell = shell_.lock();
			if(lshell && lshell->focus().lock() == sharedFromThis())
				lshell->blur();
		}

		void Control::onResizeInternal(const Objects::RectangleF &)
		{
		}

		bool Control::onSetCursorInternal()
		{
			if(cursor_)
			{
				SetCursor(*cursor_);
				return true;
			}
			return false;
		}

		const ShellWeakPtr<> &Control::shell() const
		{
			return shell_;
		}

		Slot *Control::parent() const
		{
			return parent_;
		}

		const ControlID &Control::id() const
		{
			return id_;
		}

		const Objects::RectangleF &Control::rect() const
		{
			return rect_;
		}

		void Control::rect(const Objects::RectangleF &irect)
		{
			rect_ = irect;
			onResizeInternal(irect);
		}

		bool Control::hasFocus()
		{
			ShellPtr<> lshell = shell_.lock();
			if(lshell)
				return lshell->focus().lock() == sharedFromThis();
			return false;
		}

		void Control::shell(const ShellWeakPtr<> &ishell)
		{
			shell_ = ishell;
		}

		const Resources::ResourcePtr<Resources::Cursor> &Control::cursor() const
		{
			return cursor_;
		}

		void Control::cursor(const Resources::ResourcePtr<Resources::Cursor> &icursor)
		{
			cursor_ = icursor;
			if(icursor)
				SetCursor(*icursor);
		}

		bool Control::focusable() const
		{
			return true;
		}

		Shell::Shell() // no parent
		{
		}

		Shell::Shell(const ShellWeakPtr<> &iparent) // with parent
			: parent_(iparent)
		{
		}

		Shell::~Shell()
		{
		}

		void Shell::fireEvent(const std::vector<ControlWeakPtr<>> &controls, ControlEvent (Control::*ev), ControlEventArg arg)
		{
			for(auto it = std::begin(controls); it != std::end(controls); ++ it)
			{
				ControlPtr<> lctl = it->lock();
				if(lctl)
				{
					if(arg.shellClientPoint != Objects::PointF::invalid)
						arg.controlPoint = translatePointIn(arg.shellClientPoint, lctl->rect());
					(lctl.get()->*ev)(arg);
					if(!arg.isPropagatable())
						break;
				}
			}
		}

		ShellPtr<> Shell::sharedFromThis()
		{
			return ShellPtr<>(shared_from_this());
		}

		void Shell::redraw()
		{
		}

		const ShellWeakPtr<> &Shell::parent() const
		{
			return parent_;
		}

		HWND Shell::hwnd() const
		{
			return hwnd_;
		}

		Objects::SizeF Shell::clientSize() const
		{
			// TODO: consider DPI
			RECT rc;
			GetClientRect(hwnd_, &rc);
			return Objects::SizeF(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top));
		}

		const ControlWeakPtr<> &Shell::focus() const
		{
			return focus_;
		}

		void Shell::focus(const ControlWeakPtr<> &ifocus)
		{
			ControlPtr<> lfocus = focus_.lock(), lifocus = ifocus.lock();
			if(lifocus && lifocus->focusable() && lfocus != lifocus)
			{
				if(lfocus)
					fireEvent(std::vector<ControlWeakPtr<>>(1, focus_), &Control::onBlur, ControlEventArg());
				focus_ = ifocus;
				fireEvent(std::vector<ControlWeakPtr<>>(1, ifocus), &Control::onFocus, ControlEventArg());
			}
		}

		void Shell::blur()
		{
			if(focus_.lock())
				fireEvent(std::vector<ControlWeakPtr<>>(1, focus_), &Control::onBlur, ControlEventArg());
			focus_ = nullptr;
		}

		void Shell::hwnd(HWND ihwnd)
		{
			hwnd_ = ihwnd;
		}
	}
}
