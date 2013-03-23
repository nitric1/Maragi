#include "Common.h"

#include "Global.h"

namespace Maragi
{
    Environment::Environment()
    {
        inst_ = GetModuleHandle(nullptr);
    }

    GlobalInitializerManager::~GlobalInitializerManager()
    {
        for(auto it = std::begin(inits); it != std::end(inits); ++ it)
            it->second->uninit();
    }

    void GlobalInitializerManager::add(const std::shared_ptr<Initializer> &init)
    {
        boost::mutex::scoped_lock sl(lock);
        if(inits.find(init->getName()) != std::end(inits))
            return;
        inits.insert(std::make_pair(init->getName(), init));
        init->init();
    }
}
