#pragma once

namespace Batang
{
    namespace Detail
    {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4425) // 'constexpr' was ignored (class literal types are not yet supported)
#endif // _MSC_VER

        template<typename T, typename Tag, T InvalidValue = T()>
        class ValueWrapper final
        {
            static_assert(!std::is_reference<T>::value, "T must not be reference type.");

        public:
            typedef T ValueType;
            static const T InvalidValue = InvalidValue;
            struct Uninitialized {};

        public:
            // for std::hash
            template<template<typename> class HashT = std::hash>
            struct Hash
            {
            private:
                HashT<ValueType> hash_;

            public:
                size_t operator ()(const ValueWrapper &value) const
                {
                    return hash_(*value);
                }
            };

        private:
            T value_;

        public:
            constexpr ValueWrapper()
                : value_(InvalidValue)
            {}
            constexpr ValueWrapper(Uninitialized) {}
            constexpr ValueWrapper(const T &value)
                : value_(value)
            {}
            constexpr ValueWrapper(T &&value)
                : value_(std::move(value))
            {}

        public:
            bool operator ==(const ValueWrapper &rhs) const
            {
                return value_ == rhs.value_;
            }
            bool operator !=(const ValueWrapper &rhs) const
            {
                return !(value_ == rhs.value_);
            }
            bool operator <(const ValueWrapper &rhs) const
            {
                return value_ < rhs.value_;
            }
            bool operator >(const ValueWrapper &rhs) const
            {
                return rhs.value_ < value_;
            }
            bool operator <=(const ValueWrapper &rhs) const
            {
                return !(rhs.value_ < value_);
            }
            bool operator >=(const ValueWrapper &rhs) const
            {
                return !(value_ < rhs.value_);
            }
            constexpr const T &operator *() const
            {
                return value_;
            }
            constexpr operator bool() const
            {
                return value_ != InvalidValue;
            }
        };

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

        // for boost::hash
        template<typename T, typename Tag, T InvalidValue>
        size_t hash_value(const ValueWrapper<T, Tag, InvalidValue> &wrapper)
        {
            return boost::hash_value(*wrapper);
        }
    }

    using Detail::ValueWrapper;
}

// OK, I know declaring std is devil...
namespace std
{
    template<typename T, typename Tag, T InvalidValue>
    struct hash<Batang::ValueWrapper<T, Tag, InvalidValue>>
        : Batang::ValueWrapper<T, Tag, InvalidValue>::Hash<>
    {};
}
