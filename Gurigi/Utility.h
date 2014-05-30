#pragma once

#include "Batang/Singleton.h"

#include "Primitives.h"
#include "Forwards.h"

namespace Gurigi
{
    template<typename Base, typename Deleter, typename T>
    class SharedPtr
    {
        static_assert(std::is_convertible<T *, Base *>::value, "T must be a derived class of Base.");

    private:
        std::shared_ptr<Base> ptr_;
        T *castPtr_;

    public:
        SharedPtr()
            : ptr_(nullptr, Deleter())
            , castPtr_(nullptr)
        {}

        SharedPtr(Base *ptr)
            : ptr_(ptr, Deleter())
            , castPtr_(dynamic_cast<T *>(ptr))
        {}

        template<typename Other>
        SharedPtr(const SharedPtr<Base, Deleter, Other> &that)
            : ptr_(that.ptr_)
            , castPtr_(dynamic_cast<T *>(that.ptr_.get()))
        {}

        template<typename Other>
        SharedPtr(SharedPtr<Base, Deleter, Other> &&that)
            : ptr_(std::move(that.ptr_))
        {
            castPtr_ = dynamic_cast<T *>(ptr_.get());
        }

        SharedPtr(const std::shared_ptr<Base> &ptr)
            : ptr_(ptr)
            , castPtr_(dynamic_cast<T *>(ptr.get()))
        {}

        SharedPtr(std::shared_ptr<Base> &&ptr)
            : ptr_(std::move(ptr))
        {
            castPtr_ = dynamic_cast<T *>(ptr_.get());
        }

    public:
        T *get() const
        {
            return castPtr_;
        }

    public:
        template<typename Other>
        SharedPtr &operator =(const SharedPtr<Base, Deleter, Other> &rhs)
        {
            ptr_ = rhs.ptr_;
            castPtr_ = dynamic_cast<T *>(ptr_.get());
            return *this;
        }

        template<typename Other>
        SharedPtr &operator =(SharedPtr<Base, Deleter, Other> &&rhs)
        {
            ptr_ = std::move(rhs.ptr_);
            castPtr_ = dynamic_cast<T *>(ptr_.get());
            return *this;
        }

        bool operator ==(const SharedPtr &rhs) const
        {
            return castPtr_ == rhs.castPtr_;
        }

        bool operator !=(const SharedPtr &rhs) const
        {
            return castPtr_ != rhs.castPtr_;
        }

        bool operator <(const SharedPtr &rhs) const
        {
            return castPtr_ < rhs.castPtr_;
        }

        bool operator >(const SharedPtr &rhs) const
        {
            return castPtr_ > rhs.castPtr_;
        }

        bool operator <=(const SharedPtr &rhs) const
        {
            return castPtr_ <= rhs.castPtr_;
        }

        bool operator >=(const SharedPtr &rhs) const
        {
            return castPtr_ >= rhs.castPtr_;
        }

        T *operator ->() const
        {
            return castPtr_;
        }

        explicit operator bool() const
        {
            return castPtr_ != nullptr;
        }

        bool operator !() const
        {
            return castPtr_ == nullptr;
        }

        T &operator *() const
        {
            return *castPtr_;
        }

        template<typename, typename, typename>
        friend class SharedPtr;
        template<typename, typename, typename>
        friend class WeakPtr;
    };

    template<typename Base, typename Deleter, typename T>
    class WeakPtr
    {
        static_assert(std::is_convertible<T *, Base *>::value, "T must be a derived class of Base.");

    protected:
        std::weak_ptr<Base> ptr_;

    public:
        WeakPtr()
            : ptr_()
        {}

        WeakPtr(nullptr_t)
            : ptr_()
        {}

        WeakPtr(const std::weak_ptr<Base> &ptr)
            : ptr_(ptr)
        {}

        template<typename Other>
        WeakPtr(const SharedPtr<Base, Deleter, Other> &that)
            : ptr_(that.ptr_)
        {}

        template<typename Other>
        WeakPtr(const WeakPtr<Base, Deleter, Other> &that)
            : ptr_(that.ptr_)
        {}

    public:
        SharedPtr<Base, Deleter, T> lock() const
        {
            return ptr_.lock();
        }

    public:
        template<typename Other>
        WeakPtr &operator =(const SharedPtr<Base, Deleter, Other> &rhs)
        {
            ptr_ = rhs.ptr_;
            return *this;
        }

        template<typename Other>
        WeakPtr &operator =(const WeakPtr<Base, Deleter, Other> &rhs)
        {
            ptr_ = rhs.ptr_;
            return *this;
        }

        template<typename Other>
        bool operator ==(const WeakPtr<Base, Deleter, Other> &rhs)
        {
            return lock() == rhs.lock();
        }

        template<typename Other>
        bool operator !=(const WeakPtr<Base, Deleter, Other> &rhs)
        {
            return lock() != rhs.lock();
        }

        template<typename Base, typename Deleter, typename Other>
        friend class WeakPtr;
    };

    struct ControlPtrDeleter
    {
        void operator ()(Control *) const;
    };

    template<typename T = Control>
    using ControlPtr = SharedPtr<Control, ControlPtrDeleter, T>;
    template<typename T = Control>
    using ControlWeakPtr = WeakPtr<Control, ControlPtrDeleter, T>;

    struct ShellPtrDeleter
    {
        void operator ()(Shell *) const;
    };

    template<typename T = Shell>
    using ShellPtr = SharedPtr<Shell, ShellPtrDeleter, T>;
    template<typename T = Shell>
    using ShellWeakPtr = WeakPtr<Shell, ShellPtrDeleter, T>;

    class ControlManager final : public Batang::Singleton<ControlManager>
    {
    private:
        std::unordered_map<
            ControlId::ByValue<Batang::ValueWrapper::Operators::SingleBind<Batang::ValueWrapper::Operators::EqualityOperator>::Type>,
            ControlWeakPtr<>> controls_;

        ControlId nextId_;

    private:
        ControlManager();
        ~ControlManager();

    public:
        ControlId getNextId();
        void add(const ControlId &, const ControlWeakPtr<> &);
        ControlWeakPtr<> find(ControlId);
        void remove(ControlId);

        friend class Batang::Singleton<ControlManager>;
    };

    class ShellManager final : public Batang::Singleton<ShellManager>
    {
    private:
        std::map<HWND, ShellWeakPtr<>> shells_;

        uint32_t nextId_;

    private:
        ShellManager();
        ~ShellManager();

    public:
        static longptr_t __stdcall windowProc(HWND, unsigned, uintptr_t, longptr_t);
        static intptr_t __stdcall dialogProc(HWND, unsigned, uintptr_t, longptr_t);

    public:
        std::wstring getNextClassName();
        void add(HWND, const ShellWeakPtr<> &);
        ShellWeakPtr<> find(HWND);
        void remove(HWND);

        friend class Batang::Singleton<ShellManager>;
    };
}
