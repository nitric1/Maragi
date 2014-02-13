#pragma once

#include "Singleton.h"

namespace Batang
{
#ifdef _WIN32
    class Win32Environment : public Singleton<Win32Environment>
    {
    private:
        HINSTANCE inst_; // Main instance
        std::unordered_set<HINSTANCE> dllInstances_; // DLLs' instance

    private:
        Win32Environment();

    public:
        void addDllInstance(HINSTANCE dllInst)
        {
            dllInstances_.insert(dllInst);
        }

        void removeDllInstance(HINSTANCE dllInst)
        {
            dllInstances_.erase(dllInst);
        }

        HINSTANCE getInstance() const
        {
            return inst_;
        }

        const std::unordered_set<HINSTANCE> &getDllInstances() const
        {
            return dllInstances_;
        }

        friend class Singleton<Win32Environment>;
    };
#endif

    class Initializer
    {
    public:
        virtual std::string getName() const
        {
            return typeid(*this).name();
        }

        virtual void init() = 0;
        virtual void uninit() = 0;
    };

    class SimpleInitializer : public Initializer
    {
    private:
        std::string name_;
        std::function<void ()> initFn_, uninitFn_;

    public:
        SimpleInitializer(const std::string &name, const std::function<void ()> &initFn, const std::function<void ()> &uninitFn)
            : name_(name), initFn_(initFn), uninitFn_(uninitFn)
        {}

        virtual std::string getName() const
        {
            return name_;
        }

        virtual void init()
        {
            initFn_();
        }

        virtual void uninit()
        {
            uninitFn_();
        }
    };

    class ScopedInitializer
    {
    private:
        bool uninitialized_;
        std::function<void()> uninitFn_;

    public:
        template<typename InitFn>
        ScopedInitializer(InitFn &&initFn, const std::function<void()> &uninitFn)
            : uninitFn_(uninitFn)
            , uninitialized_(false)
        {
            initFn();
        }

        ~ScopedInitializer()
        {
            uninit();
        }

        void uninit()
        {
            if(!uninitialized_)
            {
                uninitFn_();
                uninitialized_ = true;
            }
        }
    };

    class GlobalInitializerManager : public Batang::Singleton<GlobalInitializerManager>
    {
    private:
        std::map<std::string, std::shared_ptr<Initializer>> inits_;
        std::mutex lock_;

    private:
        GlobalInitializerManager()
        {}
        ~GlobalInitializerManager();

    private:
        GlobalInitializerManager(const GlobalInitializerManager &) = delete;

    public:
        void add(const std::shared_ptr<Initializer> &);
        void add(const std::string &name, const std::function<void ()> &initFn, const std::function<void ()> &uninitFn) // using SimpleInitializer
        {
            add(std::shared_ptr<Initializer>(new SimpleInitializer(name, initFn, uninitFn)));
        }

        friend class Batang::Singleton<GlobalInitializerManager>;
    };
}
