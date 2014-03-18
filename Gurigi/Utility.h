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

        SharedPtr(Base *iptr)
            : ptr_(iptr, Deleter())
            , castPtr_(dynamic_cast<T *>(iptr))
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

        SharedPtr(const std::shared_ptr<Base> &iptr)
            : ptr_(iptr)
            , castPtr_(dynamic_cast<T *>(iptr.get()))
        {}

        SharedPtr(std::shared_ptr<Base> &&iptr)
            : ptr_(std::move(iptr))
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
        template<typename, typename, typename, typename>
        friend class WeakPtr;
    };

    template<typename Base, typename Deleter, typename SharedPtrReturn, typename T>
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

        WeakPtr(const std::weak_ptr<Base> &iptr)
            : ptr_(iptr)
        {}

        template<typename Other>
        WeakPtr(const SharedPtr<Base, Deleter, Other> &that)
            : ptr_(that.ptr_)
        {}

        template<typename Other>
        WeakPtr(const WeakPtr<Base, Deleter, SharedPtrReturn, Other> &that)
            : ptr_(that.ptr_)
        {}

    public:
        SharedPtrReturn lock() const
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
        WeakPtr &operator =(const WeakPtr<Base, Deleter, SharedPtrReturn, Other> &rhs)
        {
            ptr_ = rhs.ptr_;
            return *this;
        }

        template<typename Other>
        bool operator ==(const WeakPtr<Base, Deleter, SharedPtrReturn, Other> &rhs)
        {
            return lock() == rhs.lock();
        }

        template<typename Other>
        bool operator !=(const WeakPtr<Base, Deleter, SharedPtrReturn, Other> &rhs)
        {
            return lock() != rhs.lock();
        }

        template<typename Base, typename Deleter, typename SharedPtrReturn, typename Other>
        friend class WeakPtr;
    };

    struct ControlPtrDeleter
    {
        void operator ()(Control *) const;
    };

    template<typename = Control>
    class ControlPtr;
    template<typename = Control>
    class ControlWeakPtr;

    template<typename T>
    class ControlPtr : public SharedPtr<Control, ControlPtrDeleter, T>
    {
    public:
        ControlPtr()
            : SharedPtr<Control, ControlPtrDeleter, T>()
        {}

        ControlPtr(Control *iptr)
            : SharedPtr<Control, ControlPtrDeleter, T>(iptr)
        {}

        template<typename Other>
        ControlPtr(const ControlPtr<Other> &that)
            : SharedPtr<Control, ControlPtrDeleter, T>(that)
        {}

        template<typename Other>
        ControlPtr(ControlPtr<Other> &&that)
            : SharedPtr<Control, ControlPtrDeleter, T>(std::move(that))
        {}

        ControlPtr(const std::shared_ptr<Control> &iptr)
            : SharedPtr<Control, ControlPtrDeleter, T>(iptr)
        {}

        ControlPtr(std::shared_ptr<Control> &&iptr)
            : SharedPtr<Control, ControlPtrDeleter, T>(std::move(iptr))
        {}

        friend class Control;
        template<typename>
        friend class ControlPtr;
        template<typename>
        friend class ControlWeakPtr;
    };

    template<typename T>
    class ControlWeakPtr : public WeakPtr<Control, ControlPtrDeleter, ControlPtr<T>, T>
    {
    public:
        ControlWeakPtr()
            : WeakPtr<Control, ControlPtrDeleter, ControlPtr<T>, T>()
        {}

        ControlWeakPtr(nullptr_t)
            : WeakPtr<Control, ControlPtrDeleter, ControlPtr<T>, T>(nullptr)
        {}

        ControlWeakPtr(const std::weak_ptr<Control> &iptr)
            : WeakPtr<Control, ControlPtrDeleter, ControlPtr<T>, T>(iptr)
        {}

        template<typename Other>
        ControlWeakPtr(const ControlPtr<Other> &that)
            : WeakPtr<Control, ControlPtrDeleter, ControlPtr<T>, T>(that)
        {}

        template<typename Other>
        ControlWeakPtr(const ControlWeakPtr<Other> &that)
            : WeakPtr<Control, ControlPtrDeleter, ControlPtr<T>, T>(that)
        {}

        template<typename Other>
        friend class ControlWeakPtr;
    };

    struct ShellPtrDeleter
    {
        void operator ()(Shell *) const;
    };

    template<typename = Shell>
    class ShellPtr;
    template<typename = Shell>
    class ShellWeakPtr;

    template<typename T>
    class ShellPtr : public SharedPtr<Shell, ShellPtrDeleter, T>
    {
    public:
        ShellPtr()
            : SharedPtr<Shell, ShellPtrDeleter, T>()
        {}

        ShellPtr(Shell *iptr)
            : SharedPtr<Shell, ShellPtrDeleter, T>(iptr)
        {}

        template<typename Other>
        ShellPtr(const ShellPtr<Other> &that)
            : SharedPtr<Shell, ShellPtrDeleter, T>(that)
        {}

        template<typename Other>
        ShellPtr(ShellPtr<Other> &&that)
            : SharedPtr<Shell, ShellPtrDeleter, T>(std::move(that))
        {}

        ShellPtr(const std::shared_ptr<Shell> &iptr)
            : SharedPtr<Shell, ShellPtrDeleter, T>(iptr)
        {}

        ShellPtr(std::shared_ptr<Shell> &&iptr)
            : SharedPtr<Shell, ShellPtrDeleter, T>(std::move(iptr))
        {}

        friend class Shell;
        template<typename Other>
        friend class ShellPtr;
        template<typename Other>
        friend class ShellWeakPtr;
    };

    template<typename T>
    class ShellWeakPtr : public WeakPtr<Shell, ShellPtrDeleter, ShellPtr<T>, T>
    {
    public:
        ShellWeakPtr()
            : WeakPtr<Shell, ShellPtrDeleter, ShellPtr<T>, T>()
        {}

        ShellWeakPtr(nullptr_t)
            : WeakPtr<Shell, ShellPtrDeleter, ShellPtr<T>, T>(nullptr)
        {}

        ShellWeakPtr(const std::weak_ptr<Shell> &iptr)
            : WeakPtr<Shell, ShellPtrDeleter, ShellPtr<T>, T>(iptr)
        {}

        template<typename Other>
        ShellWeakPtr(const ShellPtr<Other> &that)
            : WeakPtr<Shell, ShellPtrDeleter, ShellPtr<T>, T>(that)
        {}

        template<typename Other>
        ShellWeakPtr(const ShellWeakPtr<Other> &that)
            : WeakPtr<Shell, ShellPtrDeleter, ShellPtr<T>, T>(that)
        {}

        template<typename Other>
        friend class ShellWeakPtr;
    };

    class ControlManager : public Batang::Singleton<ControlManager>
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
        ControlId getNextID();
        void add(const ControlId &, const ControlWeakPtr<> &);
        ControlWeakPtr<> find(ControlId);
        void remove(ControlId);

        friend class Batang::Singleton<ControlManager>;
    };

    class ShellManager : public Batang::Singleton<ShellManager>
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
