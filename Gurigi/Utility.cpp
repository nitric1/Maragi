#include "Common.h"

#include "Constants.h"
#include "Utility.h"
#include "Window.h"

namespace Gurigi
{
    void ControlPtrDeleter::operator ()(Control *ptr) const
    {
        delete ptr;
    }

    void ShellPtrDeleter::operator ()(Shell *ptr) const
    {
        delete ptr;
    }

    ControlManager::ControlManager()
        : nextId_()
    {
    }

    ControlManager::~ControlManager()
    {
    }

    ControlId ControlManager::getNextId()
    {
        nextId_ = ControlId(*nextId_ + 1);
        return nextId_;
    }

    void ControlManager::add(const ControlId &id, const ControlWeakPtr<> &control)
    {
        controls_.insert(std::make_pair(id, control));
    }

    ControlWeakPtr<> ControlManager::find(ControlId id)
    {
        auto it = controls_.find(id);
        if(it == std::end(controls_))
            return nullptr;
        return it->second;
    }

    void ControlManager::remove(ControlId id)
    {
        controls_.erase(id);
    }

    ShellManager::ShellManager()
        : nextId_()
    {
    }

    ShellManager::~ShellManager()
    {
    }

    longptr_t __stdcall ShellManager::windowProc(HWND hwnd, unsigned message, uintptr_t wParam, longptr_t lParam)
    {
        ShellPtr<> lshell;
        {
            auto &shells = ShellManager::instance().shells_;
            auto it = shells.find(hwnd);
            if(it == std::end(shells))
            {
                if(message == WM_NCCREATE)
                {
                    ShellWeakPtr<> *shell = reinterpret_cast<ShellWeakPtr<> *>(reinterpret_cast<CREATESTRUCT *>(lParam)->lpCreateParams);
                    if(shell != nullptr)
                    {
                        ShellManager::instance().add(hwnd, *shell);
                        ShellPtr<> lshell = shell->lock();
                        if(lshell)
                            return lshell->procMessage(hwnd, message, wParam, lParam);
                    }
                }
                return DefWindowProcW(hwnd, message, wParam, lParam);
            }

            lshell = it->second.lock();
        }
        if(lshell)
            return lshell->procMessage(hwnd, message, wParam, lParam);
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

    intptr_t __stdcall ShellManager::dialogProc(HWND hwnd, unsigned message, uintptr_t wParam, longptr_t lParam)
    {
        ShellPtr<> lshell;
        {
            auto &shells = ShellManager::instance().shells_;
            auto it = shells.find(hwnd);
            if(it == std::end(shells))
            {
                if(message == WM_INITDIALOG)
                {
                    ShellWeakPtr<> *shell = reinterpret_cast<ShellWeakPtr<> *>(lParam);
                    if(shell != nullptr)
                    {
                        ShellManager::instance().add(hwnd, *shell);
                        ShellPtr<> lshell = shell->lock();
                        if(lshell)
                            return lshell->procMessage(hwnd, message, wParam, lParam);
                    }
                }
                return 0;
            }

            lshell = it->second.lock();
        }
        if(lshell)
            return lshell->procMessage(hwnd, message, wParam, lParam);
        return 0;
    }

    std::wstring ShellManager::getNextClassName()
    {
        ++ nextId_;
        return (boost::wformat(Constants::SHELL_CLASSNAME_TEMPLATE) % nextId_).str();
    }

    void ShellManager::add(HWND hwnd, const ShellWeakPtr<> &shell)
    {
        shells_.insert(std::make_pair(hwnd, shell));
    }

    ShellWeakPtr<> ShellManager::find(HWND hwnd)
    {
        auto it = shells_.find(hwnd);
        if(it == std::end(shells_))
            return nullptr;
        return it->second;
    }

    void ShellManager::remove(HWND hwnd)
    {
        shells_.erase(hwnd);
    }

    Win32DpiUtil::Win32DpiUtil()
    {
        moduleUser32_ = LoadLibraryW(L"user32.dll");
        if(moduleUser32_ == nullptr)
        {
            auto lastError = GetLastError();
            throw(std::runtime_error((boost::format("user32.dll cannot be loaded; GetLastError=%08X") % lastError).str()));
        }

        moduleShcore_ = LoadLibraryW(L"shcore.dll");
        if(moduleShcore_ == nullptr)
        {
            auto lastError = GetLastError();
            throw(std::runtime_error((boost::format("shcore.dll cannot be loaded; GetLastError=%08X") % lastError).str()));
        }

        getDpiForWindowFn_ = reinterpret_cast<GetDpiForWindowPtr>(GetProcAddress(moduleUser32_, "GetDpiForWindow"));
        getDpiForMonitorFn_ = reinterpret_cast<GetDpiForMonitorPtr>(GetProcAddress(moduleShcore_, "GetDpiForMonitor"));
    }

    Win32DpiUtil::~Win32DpiUtil()
    {
        if(moduleUser32_)
        {
            FreeLibrary(moduleUser32_);
        }

        if(moduleShcore_)
        {
            FreeLibrary(moduleShcore_);
        }
    }

    uint32_t Win32DpiUtil::getDpiForWindow(HWND hwnd)
    {
        if(getDpiForWindowFn_ != nullptr && hwnd != nullptr)
        {
            return getDpiForWindowFn_(hwnd);
        }

        if(getDpiForMonitorFn_ != nullptr)
        {
            HMONITOR monitor = nullptr;
            if(hwnd != nullptr)
            {
                monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
            }
            if(monitor == nullptr)
            {
                monitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
            }

            UINT dpiX = 0, dpiY = 0;
            const int MDT_EFFECTIVE_DPI = 0; // from MONITOR_DPI_TYPE in <shellscalingapi.h>
            HRESULT result = getDpiForMonitorFn_(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

            if(SUCCEEDED(result))
            {
                return dpiY;
            }
        }

        HDC hdc = GetDC(hwnd);
        uint32_t dpi = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(hwnd, hdc);

        return dpi;
    }
}
