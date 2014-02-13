#include "Common.h"

#include "Thread.h"

namespace Gurigi
{
    std::atomic<Batang::ThreadTaskPool *> UiThread::uiThread_;
}
