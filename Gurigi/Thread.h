#pragma once

#include "../Batang/Thread.h"

namespace Gurigi
{
    // TODO: minimize lock
    class UiThread
    {
    public:
        static bool post(const std::function<void ()> &task)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto luiThread = uiThread_.lock();
            if(!luiThread)
                return false;
            luiThread->post(task);
            return true;
        }
        template<typename Func>
        static auto invoke(Func &&fn) -> boost::optional<std::future<decltype(fn())>>
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto luiThread = uiThread_.lock();
            if(!luiThread)
                return {};
            return luiThread->invoke(fn);
        }

    private:
        static void set(std::weak_ptr<Batang::ThreadTaskPool> uiThread)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            uiThread_ = std::move(uiThread);
        }
        static std::weak_ptr<Batang::ThreadTaskPool> exchange(std::weak_ptr<Batang::ThreadTaskPool> uiThread)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto oldUiThread = uiThread_;
            uiThread_ = std::move(uiThread);
            return oldUiThread;
        }

    private:
        static std::weak_ptr<Batang::ThreadTaskPool> uiThread_;
        static std::mutex mutex_;

        friend class FrameWindow;
    };
}
