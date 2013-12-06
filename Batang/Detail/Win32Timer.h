#pragma once

#include "../Thread.h"

namespace Batang
{
    class Timer;

    namespace Detail
    {
        class TimerThread : public Thread<TimerThread>
        {
        private:
            HANDLE timer_;
            std::atomic<bool> toEnd_;

        private:
            TimerThread();

        private:
            void run();

        public:
            void nextTick(const std::chrono::steady_clock::duration &duration);
            void stop();

            friend class Thread<TimerThread>;
            friend class Timer;
        };
    }
}
