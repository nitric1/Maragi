#include "Common.h"

#ifdef BATANG_TIMER_TIMERFD

#include "TimerfdTimer.h"

#include "../Timer.h"

namespace Batang
{
    namespace Detail
    {
        TimerThread::TimerThread()
            : timerfd_(timerfd_create(CLOCK_MONOTONIC, 0))
        {
            if(timerfd_ == -1)
            {
                throw(std::runtime_error("timerfd_create failed."));
            }
        }

        TimerThread::~TimerThread()
        {
            close(timerfd_);
        }

        void TimerThread::run()
        {
            uint64_t tick;
            while(read(timerfd_, &tick, sizeof(tick)) == sizeof(tick))
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
            static const int32_t SecToNsec = 1000000000;

            auto nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

            itimerspec dueTime = { { 0, 0 }, { 0, 0 } };
            clock_gettime(CLOCK_MONOTONIC, &dueTime.it_value);

            dueTime.it_value.tv_nsec += nsec % SecToNsec;
            dueTime.it_value.tv_sec += nsec / SecToNsec;
            if(dueTime.it_value.tv_nsec >= SecToNsec)
            {
                dueTime.it_value.tv_sec += dueTime.it_value.tv_nsec / SecToNsec;
                dueTime.it_value.tv_nsec %= SecToNsec;
            }

            timerfd_settime(fd, TFD_TIMER_ABSTIME, &dueTime, nullptr);
        }

        void TimerThread::stop()
        {
            toEnd_ = true;

            itimerspec cancelSpec = { { 0, 0 }, { 0, 0 } };
            timerfd_settime(fd, 0, &cancelSpec, nullptr);
        }
    }
}

#endif
