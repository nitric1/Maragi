#pragma once

template<typename FunctionType>
class ERDelegate;

template<typename FunctionType>
class ERDelegateImpl;

template<typename FunctionType>
class ERDelegateWrapper;

template<typename Fn>
struct ERDelegateFunctionTraits : ERDelegateFunctionTraits<decltype(&Fn::operator ())>
{};

#include <boost/bind.hpp>

template<typename Return, typename ...Args>
class ERDelegate<Return (Args...)>
{
public:
    virtual ~ERDelegate()
    {
    }

public:
    virtual Return invoke(Args ...) = 0;
    virtual Return operator ()(Args ...) = 0;
    virtual operator bool() = 0;
};

template<typename Return, typename ...Args>
class ERDelegateImpl<Return (Args...)> : public ERDelegate<Return (Args...)>
{
private:
    template<typename FunctionClass, typename InstanceClass>
    static std::function<Return (Args...)> makeClassFunc(
        Return (FunctionClass::*ifn)(Args...), InstanceClass *ic)
    {
        return [ifn, ic](Args ...args) { return (ic->*ifn)(args...); };
    }

    template<typename FunctionClass, typename InstanceClass>
    static std::function<Return (Args...)> makeClassFunc(
        Return (FunctionClass::*ifn)(Args...) const, const InstanceClass *ic)
    {
        return [ifn, ic](Args ...args) { return (ic->*ifn)(args...); };
    }

    template<typename FunctionClass, typename InstanceClass>
    static std::function<Return (Args...)> makeClassFunc(
        Return (FunctionClass::*ifn)(Args...) volatile, volatile InstanceClass *ic)
    {
        return [ifn, ic](Args ...args) { return (ic->*ifn)(args...); };
    }

    template<typename FunctionClass, typename InstanceClass>
    static std::function<Return (Args...)> makeClassFunc(
        Return (FunctionClass::*ifn)(Args...) const volatile, const volatile InstanceClass *ic)
    {
        return [ifn, ic](Args ...args) { return (ic->*ifn)(args...); };
    }

private:
    std::function<Return (Args...)> fn;

public:
    ERDelegateImpl()
    {
    }

    ERDelegateImpl(const ERDelegateImpl<Return (Args...)> &obj)
        : fn(obj.fn)
    {
    }

    ERDelegateImpl(ERDelegateImpl<Return (Args...)> &&obj)
        : fn(std::move(obj.fn))
    {
    }

    ERDelegateImpl(Return (*ifn)(Args...))
        : fn(ifn)
    {
    }

    template<typename FunctionClass, typename InstanceClass>
    ERDelegateImpl(Return (FunctionClass::*ifn)(Args...), InstanceClass *ic)
        : fn(makeClassFunc(ifn, ic))
    {
    }

    template<typename FunctionClass, typename InstanceClass>
    ERDelegateImpl(Return (FunctionClass::*ifn)(Args...) const, const InstanceClass *ic)
        : fn(makeClassFunc(ifn, ic))
    {
    }

    template<typename FunctionClass, typename InstanceClass>
    ERDelegateImpl(Return (FunctionClass::*ifn)(Args...) volatile, volatile InstanceClass *ic)
        : fn(makeClassFunc(ifn, ic))
    {
    }

    template<typename FunctionClass, typename InstanceClass>
    ERDelegateImpl(Return (FunctionClass::*ifn)(Args...) const volatile, const volatile InstanceClass *ic)
        : fn(makeClassFunc(ifn, ic))
    {
    }

    template<typename Functor>
    ERDelegateImpl(Functor ifn)
        : fn(ifn)
    {
    }

    virtual ~ERDelegateImpl()
    {
    }

public:
    ERDelegateImpl<Return (Args...)> &operator =(std::nullptr_t)
    {
        fn.clear();
        return *this;
    }

    ERDelegateImpl<Return (Args...)> &operator =(const ERDelegateImpl<Return (Args...)> &obj)
    {
        fn = obj.fn;
        return *this;
    }

    ERDelegateImpl<Return (Args...)> &operator =(ERDelegateImpl<Return (Args...)> &&obj)
    {
        fn = std::move(obj.fn);
        return *this;
    }

    ERDelegateImpl<Return (Args...)> &operator =(Return (*ifn)(Args...))
    {
        fn = ifn;
        return *this;
    }

    template<typename FunctionClass, typename InstanceClass>
    ERDelegateImpl<Return (Args...)> &operator =(std::pair<Return (FunctionClass::*)(Args...), InstanceClass *> ifn)
    {
        fn = makeClassFunc(ifn.first, ifn.second);
        return *this;
    }

    template<typename FunctionClass, typename InstanceClass>
    ERDelegateImpl<Return (Args...)> &operator =(std::pair<Return (FunctionClass::*)(Args...) const, const InstanceClass *> ifn)
    {
        fn = makeClassFunc(ifn.first, ifn.second);
        return *this;
    }

    template<typename FunctionClass, typename InstanceClass>
    ERDelegateImpl<Return (Args...)> &operator =(std::pair<Return (FunctionClass::*)(Args...) volatile, volatile InstanceClass *> ifn)
    {
        fn = makeClassFunc(ifn.first, ifn.second);
        return *this;
    }

    template<typename FunctionClass, typename InstanceClass>
    ERDelegateImpl<Return (Args...)> &operator =(std::pair<Return (FunctionClass::*)(Args...) const volatile, const volatile InstanceClass *> ifn)
    {
        fn = makeClassFunc(ifn.first, ifn.second);
        return *this;
    }

    template<typename Functor>
    ERDelegateImpl<Return (Args...)> &operator =(Functor ifn)
    {
        fn = ifn;
        return *this;
    }

public:
    virtual Return invoke(Args ...args) override
    {
        return fn(args...);
    }

    virtual Return operator ()(Args ...args) override
    {
        return fn(args...);
    }

    virtual operator bool() override
    {
        return static_cast<bool>(fn);
    }
};

template<typename Return, typename ...Args>
class ERDelegateWrapper<Return (Args...)> : public ERDelegate<Return (Args...)>
{
private:
    std::shared_ptr<ERDelegate<Return (Args...)>> dg;

public:
    ERDelegateWrapper()
    {}

    ERDelegateWrapper(std::nullptr_t)
    {}

    ERDelegateWrapper(const std::shared_ptr<ERDelegate<Return (Args...)>> &idg)
        : dg(idg)
    {}

    ERDelegateWrapper(std::shared_ptr<ERDelegate<Return (Args...)>> &&idg)
        : dg(std::move(idg))
    {}

    ERDelegateWrapper(const ERDelegateWrapper &that)
        : dg(that.dg)
    {}

    ERDelegateWrapper(ERDelegateWrapper &&that)
        : dg(std::move(that.dg))
    {}

public:
    ERDelegateWrapper &operator =(std::nullptr_t)
    {
        dg = nullptr;
        return *this;
    }

    ERDelegateWrapper &operator =(const std::shared_ptr<ERDelegate<Return (Args...)>> &rhs)
    {
        dg = rhs;
        return *this;
    }

    ERDelegateWrapper &operator =(std::shared_ptr<ERDelegate<Return (Args...)>> &&rhs)
    {
        dg = std::move(rhs);
        return *this;
    }

    ERDelegateWrapper &operator =(const ERDelegateWrapper &rhs)
    {
        dg = rhs.dg;
        return *this;
    }

    ERDelegateWrapper &operator =(ERDelegateWrapper &&rhs)
    {
        dg = std::move(rhs.dg);
        return *this;
    }

public:
    virtual Return invoke(Args ...args) override
    {
        if(!dg)
            throw(std::bad_function_call());
        return dg->invoke(args...);
    }

    virtual Return operator ()(Args ...args) override
    {
        if(!dg)
            throw(std::bad_function_call());
        return (*dg)(args...);
    }

    virtual operator bool() override
    {
        return dg && *dg;
    }

    const std::shared_ptr<ERDelegate<Return (Args...)>> &get()
    {
        return dg;
    }

    operator std::shared_ptr<ERDelegate<Return (Args...)>>()
    {
        return dg;
    }
};

template<typename Return, typename ...Args>
struct ERDelegateFunctionTraits<Return (*)(Args...)>
{
    enum
    {
        arity = sizeof...(Args)
    };

    typedef Return ResultType;
    typedef Return result_type; // for boost

    template<size_t I>
    struct Arg
    {
        typedef typename std::tuple_element<I, std::tuple<Args...>>::type type;
        typedef type Type;
    };

    template<size_t I>
    struct arg : Arg<I>
    {};

    typedef Return (FunctionType)(Args...);
};

template<typename FunctionClass, typename Return, typename ...Args>
struct ERDelegateFunctionTraits<Return (FunctionClass::*)(Args...)>
    : ERDelegateFunctionTraits<Return (*)(Args...)>
{
    typedef FunctionClass ClassType;
};

template<typename FunctionClass, typename Return, typename ...Args>
struct ERDelegateFunctionTraits<Return (FunctionClass::*)(Args...) const>
    : ERDelegateFunctionTraits<Return (FunctionClass::*)(Args...)>
{};

template<typename FunctionClass, typename Return, typename ...Args>
struct ERDelegateFunctionTraits<Return (FunctionClass::*)(Args...) volatile>
    : ERDelegateFunctionTraits<Return (FunctionClass::*)(Args...)>
{};

template<typename FunctionClass, typename Return, typename ...Args>
struct ERDelegateFunctionTraits<Return (FunctionClass::*)(Args...) const volatile>
    : ERDelegateFunctionTraits<Return (FunctionClass::*)(Args...)>
{};
