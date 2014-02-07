#pragma once

namespace Batang
{
    namespace ValueWrapper
    {
        namespace Operators
        {
            template<typename ValueRef>
            struct NullOperator {};

            template<typename ValueRef, template<typename> class Chain>
            struct EqualityOperator: Chain<ValueRef>, private boost::equality_comparable<ValueRef>
            {
                bool operator ==(const ValueRef &rhs) const
                {
                    return *static_cast<const ValueRef *>(this)->wrapper() == *rhs.wrapper();
                }
            };

            template<typename ValueRef, template<typename> class Chain>
            struct OrderingOperator: Chain<ValueRef>, private boost::less_than_comparable<ValueRef>
            {
                bool operator <(const ValueRef &rhs) const
                {
                    return *static_cast<const ValueRef *>(this)->wrapper() < *rhs.wrapper();
                }
            };

            template<template<typename, template<typename> class> class Operator,
                template<typename> class Chain>
            struct OperatorBind
            {
                template<typename ValueRef>
                struct Type: Operator<ValueRef, Chain> {};
            };

            template<template<typename, template<typename> class> class Operator>
            struct NullBind
            {
                template<typename ValueRef>
                struct Type: Operator<ValueRef, NullOperator> {};
            };
        }

        namespace Detail
        {
            template<typename WrapperT, template<typename> class Operator = Operators::NullOperator>
            class ValueRef;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4425) // 'constexpr' was ignored (class literal types are not yet supported)
#endif // _MSC_VER

            template<typename T, typename Tag, T InvalidValue = T()>
            class Wrapper
            {
            public:
                typedef T ValueType;
                struct Uninitialized {};

                template<template<typename> class Operator = Operators::NullOperator>
                using ValueRef = Detail::ValueRef<Wrapper, Operator>;

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
                constexpr const T &operator *() const
                {
                    return value_;
                }
            };

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

            template<typename WrapperT, template<typename> class Operator>
            class ValueRef: public Operator<ValueRef<WrapperT, Operator>>
            {
            public:
                typedef WrapperT WrapperType;

            private:
                WrapperT &wrapper_;

            public:
                ValueRef(WrapperT &wrapper)
                    : wrapper_(wrapper)
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
                ValueRef &operator =(const WrapperT &rhs)
                {
                    wrapper_ = rhs;
                    return *this;
                }
                ValueRef &operator =(const ValueRef &rhs)
                {
                    wrapper_ = rhs.wrapper_;
                    return *this;
                }
                ValueRef &operator =(ValueRef &&rhs)
                {
                    wrapper_ = std::move(rhs.wrapper_);
                    return *this;
                }

                operator typename WrapperT::ValueType() const
                {
                    return *wrapper_;
                }
            };

            // for boost::hash
            template<typename T, typename Tag, T InvalidValue>
            size_t hash_value(const Wrapper<T, Tag, InvalidValue> &wrapper)
            {
                return boost::hash_value(*wrapper);
            }

            // for std::hash
            template<typename ValueRefT, template<typename> class HashT = std::hash>
            struct Hash
            {
            private:
                HashT<typename ValueRefT::WrapperType::ValueType> hash_;

            public:
                size_t operator ()(const ValueRefT &value) const
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
    template<typename ValueWrapper, template<typename> class Operator>
    struct hash<typename Batang::ValueWrapper::Detail::ValueRef<ValueWrapper, Operator>>
        : Batang::ValueWrapper::Hash<typename Batang::ValueWrapper::Detail::ValueRef<ValueWrapper, Operator>>
    {};
}
