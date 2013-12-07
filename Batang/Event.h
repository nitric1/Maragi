#pragma once

#include "Delegate.h"

namespace Batang
{
    template<typename Arg>
    class Event
    {
    private:
        boost::signals2::signal<void (const Arg &)> sig;

    public:
        Event() {}

    private:
        Event(const Event &) = delete;

    public:
        template<typename Func>
        boost::signals2::connection connect(Func fn, bool prior = false)
        {
            if(prior)
                return sig.connect(0, fn);
            return sig.connect(fn);
        }

        template<typename FunctionType>
        boost::signals2::connection connect(const ERDelegateWrapper<FunctionType> &dg, bool prior = false)
        {
            if(prior)
                return sig.connect(0, dg);
            return sig.connect(dg);
        }

        template<typename FunctionType>
        boost::signals2::connection connect(const std::shared_ptr<ERDelegate<FunctionType>> &dg, bool prior = false)
        {
            if(prior)
                return sig.connect(0, ERDelegateWrapper<FunctionType>(dg));
            return sig.connect(ERDelegateWrapper<FunctionType>(dg));
        }

        void operator ()(const Arg &arg)
        {
            sig(arg);
        }

    public:
        template<typename Func>
        boost::signals2::connection operator +=(Func rhs)
        {
            return sig.connect(rhs);
        }

        template<typename FunctionType>
        boost::signals2::connection operator +=(const ERDelegateWrapper<FunctionType> &rhs)
        {
            return sig.connect(rhs);
        }
    };

    template<>
    class Event<void>
    {
    private:
        boost::signals2::signal<void ()> sig;

    public:
        Event() {}

    private:
        Event(const Event &) = delete;

    public:
        template<typename Func>
        boost::signals2::connection connect(Func fn, bool prior = false)
        {
            if(prior)
                return sig.connect(0, fn);
            return sig.connect(fn);
        }

        template<typename FunctionType>
        boost::signals2::connection connect(const ERDelegateWrapper<FunctionType> &dg, bool prior = false)
        {
            if(prior)
                return sig.connect(0, dg);
            return sig.connect(dg);
        }

        template<typename FunctionType>
        boost::signals2::connection connect(const std::shared_ptr<ERDelegate<FunctionType>> &dg, bool prior = false)
        {
            if(prior)
                return sig.connect(0, ERDelegateWrapper<FunctionType>(dg));
            return sig.connect(ERDelegateWrapper<FunctionType>(dg));
        }

        void disconnect(boost::signals2::connection conn)
        {
            return sig.disconnect(conn);
        }

        void operator ()()
        {
            sig();
        }

    public:
        template<typename Func>
        boost::signals2::connection operator +=(Func rhs)
        {
            return sig.connect(rhs);
        }

        template<typename FunctionType>
        boost::signals2::connection operator +=(const ERDelegateWrapper<FunctionType> &rhs)
        {
            return sig.connect(rhs);
        }

        void operator -=(boost::signals2::connection rhs)
        {
            return sig.disconnect(rhs);
        }
    };
}
