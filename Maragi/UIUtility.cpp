// $Id$

#include "Common.h"

#include "UIUtility.h"
#include "Window.h"

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
			controls.insert(std::make_pair(static_cast<ControlID>(window->id), window));
		}

		Control *ControlManager::find(ControlID id)
		{
			auto it = controls.find(id);
			if(it == controls.end())
				return nullptr;
			return it->second;
		}

		void ControlManager::remove(ControlID id)
		{
			controls.erase(id);
		}
	}
}
