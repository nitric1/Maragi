#pragma once

namespace Batang
{
    template<typename T>
    class Singleton // thread-safe global
    {
    protected:
        ~Singleton() {}

    public:
        static T &instance()
        {
            // From VC++ Compiler Nov 2013 CTP, thread-safe static is supported.
            static T theInstance;
            return theInstance;
        }
        static T &inst() { return instance(); }
    };

    template<typename T>
    class SingletonShared // thread-safe global, boxed with std::shared_ptr
    {
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
        ~SingletonShared() {}

    public:
        static T &instance()
        {
            // From VC++ Compiler Nov 2013 CTP, thread-safe static is supported.
            static std::shared_ptr<T> theInstance(new T(), Deleter());
            return *theInstance;
        }
        static T &inst() { return instance(); }
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
        static T &inst() { return instance(); }
    };
}
