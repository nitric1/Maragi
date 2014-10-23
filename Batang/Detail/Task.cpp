#include "Common.h"

#include "Task.h"

namespace Batang
{
    namespace Detail
    {
        PostTask::PostTask(std::function<void ()> fn)
            : fn_(std::move(fn))
        {
        }

        void PostTask::operator ()()
        {
            fn_();
        }

        void TaskPool::push(std::unique_ptr<Task> task)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.emplace_back(std::move(task));
        }

        std::unique_ptr<Task> TaskPool::pop()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::unique_ptr<Task> task = std::move(queue_.front());
            queue_.pop_front();
            return task;
        }

        bool TaskPool::empty()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.empty();
        }
    }
}
