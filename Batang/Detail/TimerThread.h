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
            std::condition_variable timerCv_;
            std::mutex timerMutex_;
            std::chrono::steady_clock::duration nextTickDuration_;
            std::atomic<bool> toEnd_;

        private:
            TimerThread();

        public:
            ~TimerThread();

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
