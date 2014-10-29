#include "Common.h"

#include "TimerThread.h"

#include "../Timer.h"

namespace Batang
{
    namespace Detail
    {
        TimerThread::TimerThread()
        {
        }

        TimerThread::~TimerThread()
        {
        }

        void TimerThread::run()
        {
            while(!toEnd_)
            {
                {
                    std::unique_lock<std::mutex> lock(timerMutex_);
                    if(nextTickDuration_.count() == 0)
                    {
                        timerCv_.wait(lock);
                    }
                    else
                    {
                        auto duration = nextTickDuration_; // copy
                        timerCv_.wait_for(lock, duration);
                    }
                }

                if(toEnd_)
                {
                    break;
                }

                Timer::instance().onTimerTick();
            }

#if defined(_MSC_VER) && _MSC_VER <= 1800 // In VC12, thread hangs if thread routine exits normally after main() is end.
            _endthreadex(0);
#endif
        }

        void TimerThread::nextTick(std::chrono::steady_clock::duration duration)
        {
            {
                std::lock_guard<std::mutex> lock(timerMutex_);
                if(duration.count() <= 0)
                {
                    nextTickDuration_ =
                        std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::nanoseconds(1));
                }
                else
                {
                    nextTickDuration_ = duration;
                }
            }

            timerCv_.notify_one();
        }

        void TimerThread::stop()
        {
            toEnd_ = true;
            timerCv_.notify_one();
            join();
        }
    }
}
