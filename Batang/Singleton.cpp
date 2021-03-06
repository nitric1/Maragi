#include "Common.h"

#include "Singleton.h"

namespace Batang
{
    SingletonManager::~SingletonManager()
    {
        while(!instances_.empty())
        {
            instances_.erase(instances_.begin()->first);
        }
    }

    void SingletonManager::registerInstance(uint32_t destructPriority, std::shared_ptr<void> instance)
    {
        instances_.emplace(destructPriority, std::move(instance));
    }

    SingletonManager &SingletonManager::instance()
    {
        static SingletonManager theInstance;
        return theInstance;
    }
}
