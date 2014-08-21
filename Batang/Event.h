#pragma once

#include "Delegate.h"

namespace Batang
{
    template<typename Arg>
    class Event final
    {
    private:
        boost::signals2::signal<void (const Arg &)> sig_;

    public:
        Event() {}

    private:
        Event(const Event &) = delete;

    public:
        template<typename Func>
        boost::signals2::connection connect(Func fn, bool prior = false)
        {
            if(prior)
                return sig_.connect(0, std::move(fn));
            return sig_.connect(std::move(fn));
        }

        template<typename FunctionType>
        boost::signals2::connection connect(std::shared_ptr<ERDelegate<FunctionType>> dg, bool prior = false)
        {
            if(prior)
                return sig_.connect(0, ERDelegateWrapper<FunctionType>(std::move(dg)));
            return sig_.connect(ERDelegateWrapper<FunctionType>(std::move(dg)));
        }

        void disconnect(boost::signals2::connection conn)
        {
            return sig_.disconnect(conn);
        }

        void operator ()(const Arg &arg)
        {
            sig_(arg);
        }

    public:
        template<typename Func>
        boost::signals2::connection operator +=(Func rhs)
        {
            return sig_.connect(std::move(rhs));
        }

        void operator -=(boost::signals2::connection rhs)
        {
            return sig_.disconnect(rhs);
        }
    };

    template<>
    class Event<void> final
    {
    private:
        boost::signals2::signal<void ()> sig_;

    public:
        Event() {}

    private:
        Event(const Event &) = delete;

    public:
        template<typename Func>
        boost::signals2::connection connect(Func fn, bool prior = false)
        {
            if(prior)
                return sig_.connect(0, std::move(fn));
            return sig_.connect(std::move(fn));
        }

        template<typename FunctionType>
        boost::signals2::connection connect(std::shared_ptr<ERDelegate<FunctionType>> dg, bool prior = false)
        {
            if(prior)
                return sig_.connect(0, ERDelegateWrapper<FunctionType>(dg));
            return sig_.connect(ERDelegateWrapper<FunctionType>(dg));
        }

        void disconnect(boost::signals2::connection conn)
        {
            return sig_.disconnect(conn);
        }

        void operator ()()
        {
            sig_();
        }

    public:
        template<typename Func>
        boost::signals2::connection operator +=(Func rhs)
        {
            return sig_.connect(std::move(rhs));
        }

        void operator -=(boost::signals2::connection rhs)
        {
            return sig_.disconnect(rhs);
        }
    };
}
