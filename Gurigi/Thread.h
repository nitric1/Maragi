#pragma once

#include "Batang/Thread.h"

namespace Gurigi
{
    class UiThread
    {
    public:
        static bool post(const std::function<void ()> &task)
        {
            uiThread_.load(std::memory_order_relaxed)->post(task);
        }
        static bool invoke(const std::function<void ()> &task)
        {
            uiThread_.load(std::memory_order_relaxed)->invoke(task);
        }

    private:
        static void set(Batang::ThreadTaskPool *uiThread)
        {
            uiThread_.store(uiThread, std::memory_order_relaxed);
        }
        static Batang::ThreadTaskPool *exchange(Batang::ThreadTaskPool *uiThread)
        {
            return uiThread_.exchange(uiThread, std::memory_order_relaxed);
        }

    private:
        static std::atomic<Batang::ThreadTaskPool *> uiThread_;

        friend class FrameWindow;
    };
}
