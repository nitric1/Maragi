#pragma once

namespace Batang
{
    namespace DestructPriority
    {
        enum
        {
            Latest = 0,
            Later = 200,
            Late = 400,
            Normal = 500,
            Fast = 600,
            Faster = 800,
            Fastest = 1000,
        };
    }

    class SingletonManager
    {
    private:
        std::multimap<uint32_t, std::shared_ptr<void>, std::greater<uint32_t>> instances_;

    private:
        ~SingletonManager();

    private:
        void registerInstance(uint32_t destructPriority, const std::shared_ptr<void> &instance);

    private:
        static SingletonManager &instance();

        template<typename, uint32_t>
        friend class Singleton;
    };

    template<typename T, uint32_t DestPriority = DestructPriority::Normal>
    class Singleton // thread-safe global, boxed with std::shared_ptr, managed by SingletonManager
    {
        // static_assert(sizeof(T) > 0, "T must be complete type.");

    private:
        class Deleter
        {
        public:
            void operator ()(T *ptr) const
            {
                if(sizeof(T) > 0)
                    delete ptr;
            }
        };

    protected:
        Singleton()
        {
            SingletonManager::instance().registerInstance(DestPriority,
                std::shared_ptr<Singleton>(static_cast<T *>(this), Deleter()));
        }
        ~Singleton() {}

    public:
        static T &instance()
        {
            // From VC++ Compiler Nov 2013 CTP, thread-safe static is supported.
            static T *theInstance = new T();
            return *theInstance;
        }
    };

    template<typename T>
    class SingletonLocal // thread-local (using TLS)
    {
    protected:
        ~SingletonLocal() {}

    private:
        static void deleter(T *ptr)
        {
            delete ptr;
        }

    public:
        static T &instance()
        {
            static boost::thread_specific_ptr<T> ptr(deleter);
            if(ptr.get() == nullptr)
                ptr.reset(new T);
            return *ptr;
        }
    };
}
