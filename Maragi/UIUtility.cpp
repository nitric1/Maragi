// $Id$

#include "Common.h"

#include "UIUtility.h"
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

		void ControlManager::add(const ControlID &id, const ControlPtr<> &control)
		{
			controls.insert(std::make_pair(id, control));
		}

		ControlPtr<> ControlManager::find(ControlID id)
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
			if(message == WM_CREATE)
			{
				ShellPtr<> &shell = *reinterpret_cast<ShellPtr<> *>(lParam);
				ShellManager::instance().add(hwnd, shell);
				return shell->procMessage(hwnd, message, wParam, lParam);
			}

			auto &shells = ShellManager::instance().shells;
			auto it = shells.find(hwnd);
			if(it == std::end(shells))
				return 0;

			return it->second->procMessage(hwnd, message, wParam, lParam);
		}

		intptr_t __stdcall ShellManager::dialogProc(HWND hwnd, unsigned message, uintptr_t wParam, longptr_t lParam)
		{
			if(message == WM_INITDIALOG)
			{
				ShellPtr<> &shell = *reinterpret_cast<ShellPtr<> *>(lParam);
				ShellManager::instance().add(hwnd, shell);
				return shell->procMessage(hwnd, message, wParam, lParam);
			}

			auto &shells = ShellManager::instance().shells;
			auto it = shells.find(hwnd);
			if(it == std::end(shells))
				return 0;

			return it->second->procMessage(hwnd, message, wParam, lParam);
		}

		std::wstring ShellManager::getNextClassName()
		{
			++ nextID;
			return (boost::wformat(L"Maragi::UIClass%1%") % nextID).str();
		}

		void ShellManager::add(HWND hwnd, const ShellPtr<> &shell)
		{
			shells.insert(std::make_pair(hwnd, shell));
		}

		ShellPtr<> ShellManager::find(HWND hwnd)
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
