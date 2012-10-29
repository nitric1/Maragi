// $Id$

#include "Common.h"

#include "UIUtility.h"

namespace Maragi
{
	namespace UI
	{
		ControlManager::ControlManager()
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

		void ControlManager::add(Control *window)
		{
			windows.insert(std::make_pair(static_cast<ControlID>(window->id), window));
		}

		Control *ControlManager::find(ControlID id)
		{
			auto it = windows.find(id);
			if(it == windows.end())
				return nullptr;
			return it->second;
		}

		void ControlManager::remove(ControlID id)
		{
			windows.erase(id);
		}
	}
}
