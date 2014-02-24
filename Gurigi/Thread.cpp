#include "Common.h"

#include "Thread.h"

namespace Gurigi
{
    std::weak_ptr<Batang::ThreadTaskPool> UiThread::uiThread_;
    std::mutex UiThread::mutex_;
}
