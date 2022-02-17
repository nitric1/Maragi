#pragma once

#include "../Batang/Error.h"
#include "../Batang/Singleton.h"

namespace Gurigi
{
    class ComInitializer final : public Batang::Singleton<ComInitializer>
    {
    private:
        ComInitializer();
        ~ComInitializer();

        friend class Batang::Singleton<ComInitializer>;
    };

    template<typename Chain>
    class ComBase : public Chain
    {
    private:
        ulong32_t refCount_;

    public:
        ComBase() noexcept
            : refCount_(1)
        {}

        virtual ~ComBase()
        {}

        virtual HRESULT __stdcall QueryInterface(const IID &iid, void **obj)
        {
            *obj = nullptr;
            Chain::queryInterfaceImpl(iid, obj);
            if(*obj == nullptr)
                return E_NOINTERFACE;
            AddRef();
            return S_OK;
        }

        virtual ulong32_t __stdcall AddRef()
        {
            return InterlockedIncrement(&refCount_);
        }

        virtual ulong32_t __stdcall Release()
        {
            ulong32_t count = InterlockedDecrement(&refCount_);
            if(count == 0)
                delete this;
            return count;
        }

    private:
        ComBase(const ComBase &) = delete;
        ComBase &operator =(const ComBase &) = delete;
    };

    struct ComBaseListNil
    {};

    // If T is an inheritor class, use this class instead of ComBaseList.
    template<typename T, typename Chain>
    class ComBaseListSelf : public Chain
    {
    public:
        void queryInterfaceImpl(const IID &iid, void **obj) noexcept
        {
            if(iid != __uuidof(T))
                return Chain::queryInterfaceImpl(iid, obj);
            *obj = static_cast<T *>(this);
        }
    };

    template<typename T, typename Chain = ComBaseListNil>
    class ComBaseList : public T, public Chain
    {
    public:
        void queryInterfaceImpl(const IID &iid, void **obj) noexcept
        {
            if(iid != __uuidof(T))
                return Chain::queryInterfaceImpl(iid, obj);
            *obj = static_cast<T *>(this);
        }
    };

    template<typename T>
    class ComBaseList<T, ComBaseListNil> : public T
    {
    public:
        void queryInterfaceImpl(const IID &iid, void **obj) noexcept
        {
            if(iid != __uuidof(T))
                return;
            *obj = static_cast<T *>(this);
        }
    };

    template<typename T>
    class ComPtr final
    {
        static_assert(std::is_convertible<T *, IUnknown *>::value, "T must be a COM class.");

    private:
        T *ptr;

    public:
        ComPtr() noexcept
            : ptr(nullptr)
        {}

        ComPtr(nullptr_t) noexcept
            : ptr(nullptr)
        {}

        explicit ComPtr(T *iptr) noexcept
            : ptr(iptr)
        {}

        ComPtr(const ComPtr &that)
            : ptr(that.ptr)
        {
            ptr->AddRef();
        }

        template<typename U>
        ComPtr(const ComPtr<U> &that)
            : ptr(that.ptr)
        {
            static_assert(std::is_convertible<U *, T *>::value, "Type of pointer in argument must be T or a subclass of T.");
            ptr->AddRef();
        }

        ComPtr(ComPtr &&that) noexcept
            : ptr(that.ptr)
        {
            that.ptr = nullptr;
        }

        template<typename U>
        ComPtr(ComPtr<U> &&that) noexcept
            : ptr(that.ptr)
        {
            static_assert(std::is_convertible<U *, T *>::value, "Type of pointer in argument must be T or a subclass of T.");
            that.ptr = nullptr;
        }

        ~ComPtr()
        {
            if(ptr)
                ptr->Release();
        }

    public:
        T *get() const noexcept
        {
            return ptr;
        }

        T **getPtr() noexcept
        {
            return &ptr;
        }

    public:
        operator T *() const noexcept
        {
            return ptr;
        }

        T &operator *() const noexcept
        {
            BATANG_ASSERT(ptr);
            return *ptr;
        }

        T **operator &() noexcept
        {
            return &ptr;
        }

        T *const *operator &() const noexcept
        {
            return &ptr;
        }

        T *operator ->() const noexcept
        {
            return ptr;
        }

        void release()
        {
            if(ptr)
            {
                ptr->Release();
                ptr = nullptr;
            }
        }

        ComPtr &operator =(nullptr_t)
        {
            release();
            return *this;
        }

        ComPtr &operator =(const ComPtr &rhs)
        {
            if(ptr != rhs.ptr)
            {
                if(ptr)
                    ptr->Release();
                ptr = rhs.ptr;
                if(ptr)
                    ptr->AddRef();
            }
            return *this;
        }

        ComPtr &operator =(ComPtr &&rhs)
        {
            if(ptr != rhs.ptr)
            {
                if(ptr)
                    ptr->Release();
                ptr = rhs.ptr;
                rhs.ptr = nullptr;
            }
            return *this;
        }

        template<typename U>
        friend class ComPtr;
    };

    class ComException : public std::runtime_error
    {
    public:
        ComException()
            : std::runtime_error("ComException")
        {}

        explicit ComException(const std::string &message)
            : std::runtime_error(message)
        {}

        explicit ComException(const char *message)
            : std::runtime_error(message)
        {}
    };
}
