#include "Common.h"

#include "Global.h"

namespace Batang
{
#ifdef _WIN32
    Win32Environment::Win32Environment()
        : inst_(GetModuleHandleW(nullptr))
    {
    }
#endif

    GlobalInitializerManager::~GlobalInitializerManager()
    {
        for(auto it = std::begin(inits_); it != std::end(inits_); ++ it)
            it->second->uninit();
    }

    void GlobalInitializerManager::add(std::shared_ptr<Initializer> init)
    {
        std::lock_guard<std::mutex> lg(lock_);
        if(inits_.find(init->getName()) != inits_.end())
            return;
        inits_.emplace(init->getName(), std::move(init));
        init->init();
    }
}
