// $Id$

#include "Common.h"

#include "Initializer.h"

namespace Maragi
{
	void GlobalInitializerManager::add(const std::shared_ptr<Initializer> &init)
	{
		boost::mutex::scoped_lock sl(lock);
		if(inits.find(init->getName()) != std::end(inits))
			return;
		inits.insert(std::make_pair(init->getName(), init));
		init->init();
	}

	GlobalUninitializer globalUninitializer;
}
