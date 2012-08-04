// $Id$

#include "../Common.h"
#include "Utilities.h"

namespace Maragi
{
	namespace UI
	{
		WindowManager::WindowManager()
			: newID(0)
		{
		}

		WindowManager::~WindowManager()
		{
		}

		WindowID WindowManager::getNewVirtualID()
		{
			WindowID id;
			id.virtualWindow = true;
			id.id = ++ newID;
			return id;
		}
	}
}
