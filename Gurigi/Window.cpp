﻿#include "Common.h"

#include "Global.h"
#include "Layouts.h"
#include "Window.h"

namespace Gurigi
{
    Control::Control(const ControlId &iid)
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

    const ControlWeakPtr<> &Control::parent() const
    {
        return parent_;
    }

    const ControlId &Control::id() const
    {
        return id_;
    }

    const Objects::RectangleF &Control::rect() const
    {
        return rect_;
    }

    void Control::rect(const Objects::RectangleF &rect)
    {
        rect_ = rect;
        onResizeInternal(rect);
    }

    bool Control::hasFocus()
    {
        ShellPtr<> lshell = shell_.lock();
        if(lshell)
            return lshell->focus().lock() == sharedFromThis();
        return false;
    }

    void Control::shell(const ShellWeakPtr<> &shell)
    {
        shell_ = shell;
    }

    void Control::parent(const ControlWeakPtr<> &parent)
    {
        parent_ = parent;
    }

    const Resources::ResourcePtr<Resources::Cursor> &Control::cursor() const
    {
        return cursor_;
    }

    void Control::cursor(const Resources::ResourcePtr<Resources::Cursor> &cursor)
    {
        cursor_ = cursor;
        if(cursor)
            SetCursor(*cursor);
    }

    bool Control::focusable() const
    {
        return true;
    }

    Shell::Shell() // no parent
        : hwnd_(nullptr)
    {
    }

    Shell::Shell(const ShellWeakPtr<> &parent) // with parent
        : parent_(parent)
        , hwnd_(nullptr)
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
                if(arg.shellClientPoint != Objects::PointF::Invalid)
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

    void Shell::sendMessage(uint32_t message, uintptr_t wParam, longptr_t lParam)
    {
        SendMessageW(hwnd(), message, wParam, lParam);
    }

    void Shell::postMessage(uint32_t message, uintptr_t wParam, longptr_t lParam)
    {
        PostMessageW(hwnd(), message, wParam, lParam);
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
        RECT rc;
        GetClientRect(hwnd_, &rc);
        return Objects::convertSize(*this, Objects::SizeI(rc.right - rc.left, rc.bottom - rc.top));
    }

    Objects::PointF Shell::screenToClient(const Objects::PointI &pt) const
    {
        POINT wpt = {pt.x, pt.y};
        ScreenToClient(hwnd(), &wpt);
        return Objects::convertPoint(*this, Objects::PointI(wpt.x, wpt.y));
    }

    Objects::PointI Shell::clientToScreen(const Objects::PointF &pt) const
    {
        Objects::PointI converted = Objects::convertPoint(*this, pt);
        POINT wpt = {converted.x, converted.y};
        ClientToScreen(hwnd(), &wpt);
        return Objects::PointI(wpt.x, wpt.y);
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
