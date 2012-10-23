// $Id$

#include "Common.h"

#include "WindowUtility.h"

namespace Maragi
{
	namespace UI
	{
		WindowManager::WindowManager()
		{
		}

		WindowManager::~WindowManager()
		{
		}

		WindowID WindowManager::getNextID()
		{
			++ nextID.id;
			return nextID;
		}

		void WindowManager::add(Window *window)
		{
			windows.insert(std::make_pair(static_cast<WindowID>(window->id), window));
		}

		Window *WindowManager::find(WindowID id)
		{
			auto it = windows.find(id);
			if(it == windows.end())
				return nullptr;
			return it->second;
		}

		void WindowManager::remove(WindowID id)
		{
			windows.erase(id);
		}
	}
}
