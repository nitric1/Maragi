#pragma once

#include "Batang/Thread.h"

namespace Gurigi
{
    class UiThread
    {
    public:
        static bool post(const std::function<void ()> &task)
        {
            std::shared_ptr<Batang::ThreadTaskPool> luiThread;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                luiThread = uiThread_.lock();
            }
            if(luiThread)
                luiThread->post(task);
        }
        static bool invoke(const std::function<void ()> &task)
        {
            std::shared_ptr<Batang::ThreadTaskPool> luiThread;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                luiThread = uiThread_.lock();
            }
            if(luiThread)
                luiThread->invoke(task);
        }

    private:
        static void set(const std::weak_ptr<Batang::ThreadTaskPool> &uiThread)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            uiThread_ = uiThread;
        }
        static std::weak_ptr<Batang::ThreadTaskPool> exchange(const std::weak_ptr<Batang::ThreadTaskPool> &uiThread)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto oldUiThread = uiThread_;
            uiThread_ = uiThread;
            return oldUiThread;
        }

    private:
        static std::weak_ptr<Batang::ThreadTaskPool> uiThread_;
        static std::mutex mutex_;

        friend class FrameWindow;
    };
}
