// $Id$

#pragma once

#include "Controller.h"
#include "Singleton.h"

namespace Maragi
{
	class MainController : public Controller, public Singleton<MainController>
	{
	};
}
