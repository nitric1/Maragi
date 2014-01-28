#include "Common.h"

#include "Batang/Global.h"
#include "Batang/Thread.h"

#include "FrameWindow.h"
#include "Global.h"
#include "Utility.h"

namespace Gurigi
{
    const uint32_t FrameWindow::WindowStyle = WS_OVERLAPPEDWINDOW;
    const uint32_t FrameWindow::WindowStyleEx = WS_EX_CLIENTEDGE;

    FrameWindow::FrameWindow()
        : Shell()
        , bgColor_(Objects::ColorF::White)
        , inDestroy_(false)
        , capturedButtons_(0)
    {
    }

    FrameWindow::FrameWindow(const ShellWeakPtr<> &parent)
        : Shell(parent)
        , bgColor_(Objects::ColorF::White)
        , inDestroy_(false)
        , capturedButtons_(0)
    {
    }

    FrameWindow::~FrameWindow()
    {
        if(hwnd() != nullptr && !inDestroy_)
            DestroyWindow(hwnd());

        if(!className_.empty())
            UnregisterClassW(className_.c_str(), Batang::Win32Environment::instance().getInstance());
    }

    ShellPtr<FrameWindow> FrameWindow::create(
        const ShellWeakPtr<> &parent,
        const std::wstring &title,
        const Resources::ResourcePtr<Resources::Icon> &iconLarge,
        const Resources::ResourcePtr<Resources::Icon> &iconSmall,
        const Objects::ColorF &bgColor,
        const Objects::SizeF &clientSize,
        const Objects::PointI &position,
        const Objects::SizeF &minClientSize,
        const Objects::SizeF &maxClientSize
        )
    {
        std::wstring className = ShellManager::instance().getNextClassName();

        WNDCLASSEXW wcex = {0, };
        wcex.cbSize = sizeof(wcex);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra    = sizeof(void *);
        wcex.hInstance = Batang::Win32Environment::instance().getInstance();
        wcex.hIcon = *iconLarge;
        wcex.hIconSm = *iconSmall;
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = nullptr;
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = className.c_str();
        wcex.lpfnWndProc = &ShellManager::windowProc;

        RegisterClassExW(&wcex);

        ShellPtr<FrameWindow> frm = new FrameWindow(parent);

        frm->className_ = className;
        frm->initTitle_ = title;
        frm->iconLarge_ = iconLarge;
        frm->iconSmall_ = iconSmall;
        frm->bgColor_ = bgColor;
        frm->initPosition_ = position;
        frm->initClientSize_ = clientSize;
        frm->minClientSize_ = minClientSize;
        frm->maxClientSize_ = maxClientSize;

        frm->client_ = ShellLayout::create(frm);

        return frm;
    }

    bool FrameWindow::show()
    {
        return show(SW_SHOW);
    }

    bool FrameWindow::show(int32_t showCommand)
    {
        // TODO: menu
        Objects::SizeI windowSize;

        if(initPosition_ == initPosition_.Invalid)
            initPosition_ = Objects::PointI(CW_USEDEFAULT, CW_USEDEFAULT);
        if(initClientSize_ == initClientSize_.Invalid)
            windowSize = Objects::SizeF(CW_USEDEFAULT, CW_USEDEFAULT);
        else
        {
            windowSize = adjustWindowSize(initClientSize_);

            // XXX: scrollbars?
        }

        ShellPtr<> lparent = parent().lock();

        HWND hwnd = CreateWindowExW(
            WindowStyleEx,
            className_.c_str(),
            initTitle_.c_str(),
            WindowStyle,
            initPosition_.x, initPosition_.y,
            windowSize.width, windowSize.height,
            lparent ? lparent->hwnd() : nullptr,
            nullptr,
            Batang::Win32Environment::instance().getInstance(),
            &static_cast<ShellWeakPtr<>>(sharedFromThis())
            );
        if(hwnd == nullptr)
            return false;

        this->hwnd(hwnd);
        ShowWindow(hwnd, showCommand);
        UpdateWindow(hwnd);

        MSG msg;
        bool done = false;

        Batang::ThreadTaskPool *thread = Batang::ThreadTaskPool::current();

        HANDLE taskInvokedSemaphore = CreateSemaphoreW(nullptr, 1, 1, nullptr);
        auto taskInvoked = [taskInvokedSemaphore]()
        {
            ReleaseSemaphore(taskInvokedSemaphore, 1, nullptr);
        };
        auto conn = (thread->onTaskInvoked.connect(taskInvoked));

        while(!done)
        {
            if(PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if(msg.message == WM_QUIT)
                {
                    done = true;
                    break;
                }

                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            else
            {
                if(thread)
                {
                    if(MsgWaitForMultipleObjects(1, &taskInvokedSemaphore, FALSE, INFINITE, QS_ALLINPUT) == WAIT_OBJECT_0)
                    {
                        onTaskProcessable();
                    }
                }
                else
                {
                    WaitMessage();
                }
            }
        }

        thread->onTaskInvoked -= conn;
        CloseHandle(taskInvokedSemaphore);

        return true;
    }

    Objects::SizeI FrameWindow::adjustWindowSize(const Objects::SizeF &size) const
    {
        Objects::SizeI converted = Objects::convertSize(size);
        RECT rc = { 0, 0, converted.width, converted.height };
        // TODO: menu
        AdjustWindowRectEx(&rc, WindowStyle, FALSE, WindowStyleEx);
        return Objects::SizeI(rc.right - rc.left, rc.bottom - rc.top);
    }

    Objects::PointF FrameWindow::screenToClient(const Objects::PointI &pt) const
    {
        POINT wpt = { pt.x, pt.y };
        ScreenToClient(hwnd(), &wpt);
        return Objects::convertPoint(Objects::PointI(wpt.x, wpt.y));
    }

    Objects::PointI FrameWindow::clientToScreen(const Objects::PointF &pt) const
    {
        Objects::PointI converted = Objects::convertPoint(pt);
        POINT wpt = { converted.x, converted.y };
        ClientToScreen(hwnd(), &wpt);
        return Objects::PointI(wpt.x, wpt.y);
    }

    void FrameWindow::redraw()
    {
        InvalidateRect(hwnd(), nullptr, FALSE);
    }

    const ControlPtr<ShellLayout> &FrameWindow::client() const
    {
        return client_;
    }

    const Resources::ResourcePtr<Resources::Icon> &FrameWindow::iconLarge() const
    {
        return iconLarge_;
    }

    void FrameWindow::iconLarge(const Resources::ResourcePtr<Resources::Icon> &icon)
    {
        iconLarge_ = icon;
        // TODO: set icon message
    }

    const Resources::ResourcePtr<Resources::Icon> &FrameWindow::iconSmall() const
    {
        return iconSmall_;
    }

    void FrameWindow::iconSmall(const Resources::ResourcePtr<Resources::Icon> &icon)
    {
        iconSmall_ = icon;
        // TODO: set icon message
    }

    const Objects::ColorF &FrameWindow::bgColor() const
    {
        return bgColor_;
    }

    void FrameWindow::bgColor(const Objects::ColorF &ibgColor)
    {
        bgColor_ = ibgColor;
        // TODO: invalidate
    }

    Objects::SizeF FrameWindow::clientSize() const
    {
        RECT rc;
        GetClientRect(hwnd(), &rc);
        return Objects::convertSize(Objects::SizeI(rc.right - rc.left, rc.bottom - rc.top));
    }

    void FrameWindow::clientSize(const Objects::SizeF &size)
    {
        Objects::PointI pos = position();
        Objects::SizeI windowSize = adjustWindowSize(size);
        MoveWindow(hwnd(), pos.x, pos.y, windowSize.width, windowSize.height, TRUE);
    }

    Objects::SizeI FrameWindow::windowSize() const
    {
        RECT rc;
        GetWindowRect(hwnd(), &rc);
        return Objects::SizeI(rc.right - rc.left, rc.bottom - rc.top);
    }

    void FrameWindow::windowSize(const Objects::SizeI &size)
    {
        Objects::PointI pos = position();
        MoveWindow(hwnd(), pos.x, pos.y, size.width, size.height, TRUE);
    }

    Objects::PointI FrameWindow::position() const
    {
        RECT rc;
        GetWindowRect(hwnd(), &rc);
        return Objects::PointI(rc.left, rc.top);
    }

    void FrameWindow::position(const Objects::PointI &pos)
    {
        Objects::SizeI size = windowSize();
        MoveWindow(hwnd(), pos.x, pos.y, size.width, size.height, TRUE);
    }

    const Objects::SizeF &FrameWindow::minClientSize() const
    {
        return minClientSize_;
    }

    void FrameWindow::minClientSize(const Objects::SizeF &size)
    {
        minClientSize_ = size;
    }

    const Objects::SizeF &FrameWindow::maxClientSize() const
    {
        return maxClientSize_;
    }

    void FrameWindow::maxClientSize(const Objects::SizeF &size)
    {
        maxClientSize_ = size;
    }

    longptr_t FrameWindow::procMessage(HWND hwnd, unsigned message, uintptr_t wParam, longptr_t lParam)
    {
        ControlEventArg ev;
        ev.rawMessage = message;
        ev.wParam = wParam;
        ev.lParam = lParam;

        switch(message)
        {
        case WM_SIZE:
            {
                Objects::SizeF size(Objects::convertSize(Objects::SizeI(LOWORD(lParam), HIWORD(lParam))));
                context_.resize(size);
                client_->rect(Objects::RectangleF(Objects::PointF(), size));
            }
            return 0;

        case WM_DISPLAYCHANGE:
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_GETMINMAXINFO:
            {
                MINMAXINFO *mmi = reinterpret_cast<MINMAXINFO *>(lParam);
                if(minClientSize_ != Objects::SizeF::Invalid)
                {
                    Objects::SizeI min = adjustWindowSize(minClientSize_);
                    POINT pt = { min.width, min.height };
                    mmi->ptMinTrackSize = pt;
                }
                if(maxClientSize_ != Objects::SizeF::Invalid)
                {
                    Objects::SizeI max = adjustWindowSize(maxClientSize_);
                    POINT pt = { max.width, max.height };
                    mmi->ptMaxTrackSize = pt;
                }
            }
            return 0;

        case WM_PAINT:
            if(!context_)
            {
                context_.create(hwnd, clientSize());
                client_->createDrawingResources(context_);
            }

            context_.beginDraw();
            context_->SetTransform(D2D1::Matrix3x2F::Identity());
            context_->Clear(bgColor_);
            client_->draw(context_);
            if(!context_.endDraw())
            {
                client_->discardDrawingResources(context_);
            }

            ValidateRect(hwnd, nullptr);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_SETCURSOR:
            if(LOWORD(lParam) == HTCLIENT)
            {
                Objects::PointF pt = screenToClient(getCursorPos()); // TODO: get mouse point
                ControlWeakPtr<> hovered = client_->findByPoint(pt);
                ControlPtr<> lhovered = hovered.lock();
                if(lhovered && lhovered->onSetCursorInternal())
                    return 1;
            }
            break;

        case WM_MOUSEMOVE:
            {
                Objects::PointF pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
                std::vector<ControlWeakPtr<>> hovereds;
                if(captureds_.empty())
                    hovereds = client_->findReverseTreeByPoint(pt);
                else
                    hovereds = captureds_;
                if(!hovereds.empty())
                {
                    ev.shellClientPoint = pt;
                    if(hovereds.size() != prevHovereds_.size()
                        || !std::equal(hovereds.begin(), hovereds.end(), prevHovereds_.begin()))
                    {
                        // mouseout & mouseover
                        fireEvent(prevHovereds_, &Control::onMouseOut, ev);
                        fireEvent(hovereds, &Control::onMouseOver, ev);
                    }

                    fireEvent(hovereds, &Control::onMouseMove, ev);
                    prevHovereds_ = std::move(hovereds);

                    TRACKMOUSEEVENT tme = {0, };
                    tme.cbSize = sizeof(tme);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hwnd;
                    TrackMouseEvent(&tme);
                }
            }
            return 0;

        case WM_MOUSEWHEEL:
            {
                Objects::PointF pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
                std::vector<ControlWeakPtr<>> hovereds;
                if(captureds_.empty())
                    hovereds = client_->findReverseTreeByPoint(pt);
                else
                    hovereds = captureds_;
                if(!hovereds.empty())
                {
                    ev.shellClientPoint = pt;
                    ev.wheelAmount = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
                    fireEvent(hovereds, &Control::onMouseWheel, ev);
                    prevHovereds_ = std::move(hovereds);
                }
            }
            return 0;

        case WM_MOUSEHWHEEL:
            return 0;

        case WM_MOUSELEAVE:
            {
                if(!prevHovereds_.empty())
                {
                    fireEvent(prevHovereds_, &Control::onMouseOut, ev);
                    prevHovereds_.clear();
                }
            }
            return 0;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_XBUTTONDBLCLK:
            {
                Objects::PointF pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
                std::vector<ControlWeakPtr<>> hovereds;
                if(captureds_.empty())
                    hovereds = client_->findReverseTreeByPoint(pt);
                else
                    hovereds = captureds_;
                if(!hovereds.empty())
                {
                    ev.shellClientPoint = pt;

                    switch(message)
                    {
                    case WM_LBUTTONDOWN:
                    case WM_LBUTTONUP:
                    case WM_LBUTTONDBLCLK:
                        ev.buttonNum = 1;
                        break;

                    case WM_RBUTTONDOWN:
                    case WM_RBUTTONUP:
                    case WM_RBUTTONDBLCLK:
                        ev.buttonNum = 2;
                        break;

                    case WM_MBUTTONDOWN:
                    case WM_MBUTTONUP:
                    case WM_MBUTTONDBLCLK:
                        ev.buttonNum = 3;
                        break;

                    case WM_XBUTTONDOWN:
                    case WM_XBUTTONUP:
                    case WM_XBUTTONDBLCLK:
                        ev.buttonNum = 4; // TODO: X button number
                        break;
                    }

                    switch(message)
                    {
                    case WM_LBUTTONDOWN:
                    case WM_RBUTTONDOWN:
                    case WM_MBUTTONDOWN:
                    case WM_XBUTTONDOWN:
                        if(capturedButtons_ ++ == 0)
                        {
                            SetCapture(hwnd);
                            captureds_ = hovereds;
                        }
                        fireEvent(hovereds, &Control::onMouseButtonDown, ev);
                        break;

                    case WM_LBUTTONDBLCLK:
                    case WM_RBUTTONDBLCLK:
                    case WM_MBUTTONDBLCLK:
                    case WM_XBUTTONDBLCLK:
                        if(capturedButtons_ ++ == 0)
                        {
                            SetCapture(hwnd);
                            captureds_ = hovereds;
                        }
                        fireEvent(hovereds, &Control::onMouseButtonDoubleClick, ev);
                        break;

                    case WM_LBUTTONUP:
                    case WM_RBUTTONUP:
                    case WM_MBUTTONUP:
                    case WM_XBUTTONUP:
                        if(-- capturedButtons_ == 0)
                        {
                            ReleaseCapture();
                            captureds_.clear();
                        }
                        fireEvent(hovereds, &Control::onMouseButtonUp, ev);
                        break;
                    }

                    if(message == WM_LBUTTONDOWN)
                        focus(hovereds[0]);
                }
            }
            return 0;

        case WM_KEYDOWN:
            {
                ev.keyCode = wParam;
                std::vector<ControlWeakPtr<>> focuseds;
                auto lfocused = focus().lock();
                while(lfocused)
                {
                    auto parent = lfocused->parent();
                    if(!parent)
                        break;
                    focuseds.push_back(lfocused);
                    lfocused = lfocused->parent()->parent().lock();
                }
                fireEvent(focuseds, &Control::onKeyDown, ev);
            }
            return 0;

        case WM_DESTROY:
            // TODO: modeless window
            inDestroy_ = true;
            ShellManager::instance().remove(hwnd);
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
}
