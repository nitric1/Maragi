#include "Common.h"

#include "UIUtility.h"
#include "UIConstants.h"
#include "Window.h"

namespace Maragi
{
    namespace UI
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
            : nextID()
        {
        }

        ControlManager::~ControlManager()
        {
        }

        ControlID ControlManager::getNextID()
        {
            ++ nextID.id;
            return nextID;
        }

        void ControlManager::add(const ControlID &id, const ControlWeakPtr<> &control)
        {
            controls.insert(std::make_pair(id, control));
        }

        ControlWeakPtr<> ControlManager::find(ControlID id)
        {
            auto it = controls.find(id);
            if(it == std::end(controls))
                return nullptr;
            return it->second;
        }

        void ControlManager::remove(ControlID id)
        {
            controls.erase(id);
        }

        ShellManager::ShellManager()
            : nextID()
        {
        }

        ShellManager::~ShellManager()
        {
        }

        longptr_t __stdcall ShellManager::windowProc(HWND hwnd, unsigned message, uintptr_t wParam, longptr_t lParam)
        {
            ShellPtr<> lshell;
            {
                auto &shells = ShellManager::instance().shells;
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
                auto &shells = ShellManager::instance().shells;
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
            ++ nextID;
            return (boost::wformat(Constants::SHELL_CLASSNAME_TEMPLATE) % nextID).str();
        }

        void ShellManager::add(HWND hwnd, const ShellWeakPtr<> &shell)
        {
            shells.insert(std::make_pair(hwnd, shell));
        }

        ShellWeakPtr<> ShellManager::find(HWND hwnd)
        {
            auto it = shells.find(hwnd);
            if(it == std::end(shells))
                return nullptr;
            return it->second;
        }

        void ShellManager::remove(HWND hwnd)
        {
            shells.erase(hwnd);
        }
    }
}
