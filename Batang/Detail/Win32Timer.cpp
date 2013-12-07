#include "Common.h"

#ifdef _WIN32

#include "Win32Timer.h"

#include "../Timer.h"

namespace Batang
{
    namespace Detail
    {
        TimerThread::TimerThread()
            : timer_(CreateWaitableTimerW(nullptr, FALSE, nullptr))
        {
        }

        void TimerThread::run()
        {
            while(WaitForSingleObject(timer_, INFINITE) == WAIT_OBJECT_0)
            {
                if(toEnd_)
                {
                    break;
                }

                Timer::instance().onTimerTick();
            }
        }

        void TimerThread::nextTick(const std::chrono::steady_clock::duration &duration)
        {
            static const int64_t UsecToHundredNsec = 10;

            auto usec = std::chrono::duration_cast<std::chrono::microseconds>(duration);

            LARGE_INTEGER dueTime;
            dueTime.QuadPart = std::min<int64_t>(-1, -(usec.count() * UsecToHundredNsec));

            SetWaitableTimer(timer_, &dueTime, 0, nullptr, nullptr, FALSE);
        }

        void TimerThread::stop()
        {
            toEnd_ = true;
            CancelWaitableTimer(timer_);
        }
    }
}

#endif
