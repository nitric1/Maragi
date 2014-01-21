#pragma once

#include "Batang/Delegate.h"
#include "Batang/Event.h"
#include "Batang/Singleton.h"

#include "Drawing.h"
#include "Global.h"
#include "Objects.h"
#include "Primitives.h"
#include "Resources.h"
#include "Utility.h"

namespace Gurigi
{
    /*namespace Message
    {
        enum
        {
            KeyDown = WM_KEYDOWN,
            KeyUp = WM_KEYUP,
            KeyChar = WM_CHAR,
            // TODO: IME messages
            KeyImeStartComposition = WM_IME_STARTCOMPOSITION,
            KeyImeEndComposition = WM_IME_ENDCOMPOSITION,
            KeyImeComposition = WM_IME_COMPOSITION,

            MouseMove = WM_MOUSEMOVE,
            MouseLButtonDown = WM_LBUTTONDOWN,
            MouseLButtonUp = WM_LBUTTONUP,
            MouseLButtonDoubleClick = WM_LBUTTONDBLCLK,
            MouseRButtonDown = WM_RBUTTONDOWN,
            MouseRButtonUp = WM_RBUTTONUP,
            MouseRButtonDoubleClick = WM_RBUTTONDBLCLK,
            MouseMButtonDown = WM_MBUTTONDOWN,
            MouseMButtonUp = WM_MBUTTONUP,
            MouseMButtonDoubleClick = WM_MBUTTONDBLCLK,
            MouseXButtonDown = WM_XBUTTONDOWN,
            MouseXButtonUp = WM_XBUTTONUP,
            MouseXButtonDoubleClick = WM_XBUTTONDBLCLK,
            MouseWheel = WM_MOUSEWHEEL,
            MouseWheelHorizontal = WM_MOUSEHWHEEL,
        };
    }*/

    class Slot
    {
    private:
        ControlWeakPtr<> parent_;
        ControlWeakPtr<> child_;

    public:
        Slot();
        explicit Slot(const ControlWeakPtr<> &);
        virtual ~Slot();

    private:
        Slot(const Slot &) = delete;

    public:
        virtual bool attach(const ControlWeakPtr<> &);
        virtual ControlWeakPtr<> detach();

    public:
        virtual const ControlWeakPtr<> &parent() const;
        virtual void parent(const ControlWeakPtr<> &); // TODO: should be fixed
        virtual const ControlWeakPtr<> &child() const;

        friend class Control;
    };

    typedef Batang::Event<void> CommonEvent;

    struct ControlEventArg
    {
        ControlWeakPtr<> control;
        boost::posix_time::ptime time;

        // raw
        uint32_t rawMessage;
        uintptr_t wParam;
        longptr_t lParam;

        // mouse
        uint32_t buttonNum; // 0 (invalid), 1 (left), 2 (right), 3 (middle), 4, 5, ...
        Objects::PointF controlPoint;
        Objects::PointF shellClientPoint;
        //Objects::PointF screenPoint;
        float wheelAmount;

        // key
        bool altKey;
        bool ctrlKey;
        bool shiftKey;
        char keyCode; // Not in Char, ImeChar?
        wchar_t charCode; // Char, ImeChar?
        uint32_t repeated;

        ControlEventArg()
            : time(boost::posix_time::microsec_clock::local_time())
            , rawMessage(0)
            , wParam(0), lParam(0)
            , buttonNum(0)
            , controlPoint(Objects::PointF::Invalid), shellClientPoint(Objects::PointF::Invalid) // , screenPoint(Objects::PointF::invalid)
            , wheelAmount(0.0f)
            , altKey(false), ctrlKey(false), shiftKey(false)
            , keyCode('\0')
            , charCode(L'\0')
            , repeated(0)
        {}

        void stopPropagation() const
        {
            propagatable = false;
        }

        bool isPropagatable() const
        {
            return propagatable;
        }

    private:
        mutable bool propagatable;

        template<typename>
        friend class Event;
        friend class Shell;
    };

    typedef Batang::Event<ControlEventArg> ControlEvent;

    class Control : public std::enable_shared_from_this<Control>
    {
    private:
        ShellWeakPtr<> shell_;
        Slot *parent_;
        ControlID id_;
        Objects::RectangleF rect_;
        Resources::ResourcePtr<Resources::Cursor> cursor_;

    protected:
        Control(const ControlID &);
        virtual ~Control() = 0;

    private: // no implementation
        Control() = delete;
        Control(const Control &) = delete;

    protected:
        ControlPtr<> sharedFromThis();

    public:
        virtual void createDrawingResources(Drawing::Context &);
        virtual void discardDrawingResources(Drawing::Context &);
        virtual void draw(Drawing::Context &) = 0;
        virtual Objects::SizeF computeSize() = 0;
        virtual ControlWeakPtr<> findByPoint(const Objects::PointF &);
        virtual std::vector<ControlWeakPtr<>> findTreeByPoint(const Objects::PointF &); // including myself, order by root first (= leaf last)
        virtual std::vector<ControlWeakPtr<>> findReverseTreeByPoint(const Objects::PointF &); // including myself, order by leaf first (= root last)
        virtual void walk(const std::function<void (const ControlWeakPtr<> &)> &); // called from root to leaf
        virtual void walkReverse(const std::function<void (const ControlWeakPtr<> &)> &); // called from leaf to root
        virtual void redraw();
        virtual void focus();
        virtual void blur();

    public: // internal event handlers
        virtual void onResizeInternal(const Objects::RectangleF &);
        virtual bool onSetCursorInternal();

    public: // external event handlers
        ControlEvent onMouseOver;
        ControlEvent onMouseOut;
        ControlEvent onMouseMove;
        ControlEvent onMouseButtonDown;
        ControlEvent onMouseButtonDoubleClick;
        ControlEvent onMouseButtonUp;
        ControlEvent onMouseWheel;

        ControlEvent onKeyDown;
        ControlEvent onKeyUp;

        ControlEvent onFocus;
        ControlEvent onBlur;

    public:
        virtual const ShellWeakPtr<> &shell() const;
        virtual Slot *parent() const;
        virtual const ControlID &id() const;
        virtual const Objects::RectangleF &rect() const;
        virtual void rect(const Objects::RectangleF &);
        virtual bool hasFocus();

    protected:
        virtual void shell(const ShellWeakPtr<> &);
        virtual const Resources::ResourcePtr<Resources::Cursor> &cursor() const;
        virtual void cursor(const Resources::ResourcePtr<Resources::Cursor> &);
        virtual bool focusable() const;

        friend class Slot;
        friend class Shell;
        friend struct ControlPtrDeleter;
    };

    struct WindowEventArg
    {
        ShellWeakPtr<> shell;
        uint32_t message;
        uintptr_t wParam;
        longptr_t lParam;
    };

    typedef Event<WindowEventArg> WindowEvent;

    class Shell : public std::enable_shared_from_this<Shell>
    {
    private:
        ShellWeakPtr<> parent_;
        ControlWeakPtr<> focus_;
        HWND hwnd_;

    protected:
        Shell();
        explicit Shell(const ShellWeakPtr<> &);
        virtual ~Shell() = 0;

    private: // no implementation
        Shell(const Shell &) = delete;

    protected:
        ShellPtr<> sharedFromThis();

    public:
        virtual bool show() = 0;
        virtual void redraw();

    protected:
        void fireEvent(const std::vector<ControlWeakPtr<>> &, ControlEvent (Control::*), ControlEventArg);

    private:
        virtual longptr_t procMessage(HWND, uint32_t, uintptr_t, longptr_t)
        {
            return 0;
        }

    public:
        void sendMessage(uint32_t message, uintptr_t wParam, longptr_t lParam);
        void postMessage(uint32_t message, uintptr_t wParam, longptr_t lParam);

    public:
        virtual const ShellWeakPtr<> &parent() const;
        virtual HWND hwnd() const;
        virtual Objects::SizeF clientSize() const;
        virtual const ControlWeakPtr<> &focus() const;
        virtual void focus(const ControlWeakPtr<> &);
        virtual void blur();

    protected:
        virtual void hwnd(HWND);

        friend struct ShellPtrDeleter;
        friend class ShellManager;
    };
}
