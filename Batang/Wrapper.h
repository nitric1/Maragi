#pragma once

namespace Batang
{
    namespace ValueWrapper
    {
        namespace Operators
        {
            template<typename ProxyT>
            struct NullOperator {};

            template<typename ProxyT, template<typename> class Chain>
            struct EqualityOperator: Chain<ProxyT>, private boost::equality_comparable<ProxyT>
            {
                bool operator ==(const ProxyT &rhs) const
                {
                    return *static_cast<const ProxyT *>(this)->wrapper() == *rhs.wrapper();
                }
            };

            template<typename ProxyT, template<typename> class Chain>
            struct OrderingOperator: Chain<ProxyT>, private boost::less_than_comparable<ProxyT>
            {
                bool operator <(const ProxyT &rhs) const
                {
                    return *static_cast<const ProxyT *>(this)->wrapper() < *rhs.wrapper();
                }
            };

            template<template<typename, template<typename> class> class Operator,
                template<typename> class Chain>
            struct ChainBind
            {
                template<typename ProxyT>
                struct Type: Operator<ProxyT, Chain> {};
            };

            template<template<typename, template<typename> class> class Operator>
            struct SingleBind
            {
                template<typename ProxyT>
                struct Type: Operator<ProxyT, NullOperator> {};
            };
        }

        namespace Detail
        {
            template<typename WrapperT, template<typename> class Operator = Operators::NullOperator>
            class ValueProxy;
            template<typename WrapperT, template<typename> class Operator = Operators::NullOperator>
            class RefProxy;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4425) // 'constexpr' was ignored (class literal types are not yet supported)
#endif // _MSC_VER

            template<typename T, typename Tag, T InvalidValue = T()>
            class Wrapper final
            {
                static_assert(!std::is_reference<T>::value, "T must not be reference type.");

            public:
                typedef T ValueType;
                struct Uninitialized {};

                template<template<typename> class Operator = Operators::NullOperator>
                using ByValue = Detail::ValueProxy<Wrapper, Operator>;
                template<template<typename> class Operator = Operators::NullOperator>
                using ByRef = Detail::RefProxy<Wrapper, Operator>;

            private:
                T value_;

            public:
                constexpr Wrapper()
                    : value_(InvalidValue)
                {}
                constexpr Wrapper(Uninitialized) {}
                constexpr Wrapper(const T &value)
                    : value_(value)
                {}
                constexpr Wrapper(T &&value)
                    : value_(std::move(value))
                {}

            public:
                template<template<typename> class Operator = Operators::NullOperator>
                ByRef<Operator> byRef()
                {
                    return ByRef<Operator>(*this);
                }

            public:
                constexpr const T &operator *() const
                {
                    return value_;
                }
            };

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

            template<typename WrapperT, template<typename> class Operator>
            class ValueProxy final: public Operator<ValueProxy<WrapperT, Operator>>
            {
            public:
                typedef WrapperT WrapperType;
                typedef typename WrapperType::ValueType ValueType;

            private:
                WrapperT wrapper_;

            public:
                ValueProxy(const WrapperT &wrapper)
                    : wrapper_(wrapper)
                {}
                ValueProxy(WrapperT &&wrapper)
                    : wrapper_(std::forward<WrapperT>(wrapper))
                {}
                ValueProxy(const ValueProxy &proxy)
                    : wrapper_(proxy.wrapper_)
                {}
                ValueProxy(ValueProxy &&proxy)
                    : wrapper_(std::move(proxy.wrapper_))
                {}

            public:
                WrapperT &wrapper()
                {
                    return wrapper_;
                }
                const WrapperT &wrapper() const
                {
                    return wrapper_;
                }

            public:
                ValueProxy &operator =(const WrapperT &rhs)
                {
                    wrapper_ = rhs;
                    return *this;
                }
                ValueProxy &operator =(const ValueProxy &rhs)
                {
                    wrapper_ = rhs.wrapper_;
                    return *this;
                }
                ValueProxy &operator =(ValueProxy &&rhs)
                {
                    wrapper_ = std::move(rhs.wrapper_);
                    return *this;
                }

                operator ValueType() const
                {
                    return *wrapper_;
                }
            };

            template<typename WrapperT, template<typename> class Operator>
            class RefProxy final: public Operator<RefProxy<WrapperT, Operator>>
            {
            public:
                typedef WrapperT WrapperType;
                typedef typename WrapperType::ValueType ValueType;

            private:
                WrapperT &wrapper_;

            public:
                RefProxy(WrapperT &wrapper)
                    : wrapper_(wrapper)
                {}
                RefProxy(RefProxy &proxy)
                    : wrapper_(proxy.wrapper_)
                {}

            public:
                WrapperT &wrapper()
                {
                    return wrapper_;
                }
                const WrapperT &wrapper() const
                {
                    return wrapper_;
                }

            public:
                RefProxy &operator =(const WrapperT &rhs)
                {
                    wrapper_ = rhs;
                    return *this;
                }
                RefProxy &operator =(const RefProxy &rhs)
                {
                    wrapper_ = rhs.wrapper_;
                    return *this;
                }
                RefProxy &operator =(RefProxy &&rhs)
                {
                    wrapper_ = std::move(rhs.wrapper_);
                    return *this;
                }

                operator const ValueType &() const
                {
                    return *wrapper_;
                }
                const ValueType *operator &() const
                {
                    return &*wrapper_;
                }
            };

            // for boost::hash
            template<typename T, typename Tag, T InvalidValue>
            size_t hash_value(const Wrapper<T, Tag, InvalidValue> &wrapper)
            {
                return boost::hash_value(*wrapper);
            }

            // for std::hash
            template<typename ValueProxyT, template<typename> class HashT = std::hash>
            struct Hash
            {
            private:
                HashT<typename ValueProxyT::ValueType> hash_;

            public:
                size_t operator ()(const ValueProxyT &value) const
                {
                    return hash_(*value.wrapper());
                }
            };
        }

        using Detail::Wrapper;
        using Detail::Hash;
    }
}

// OK, I know declaring std is devil...
namespace std
{
    template<typename WrapperT, template<typename> class Operator>
    struct hash<typename Batang::ValueWrapper::Detail::ValueProxy<WrapperT, Operator>>
        : Batang::ValueWrapper::Hash<typename Batang::ValueWrapper::Detail::ValueProxy<WrapperT, Operator>>
    {};
}
