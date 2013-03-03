#pragma once

#include "Singleton.h"
#include "Primitives.h"
#include "UIForwards.h"

namespace Maragi
{
    namespace UI
    {
        template<typename Base, typename Deleter, typename T>
        class SharedPtr
        {
            static_assert(std::is_convertible<T *, Base *>::value, "T must be a derived class of Base.");

        private:
            std::shared_ptr<Base> ptr;
            T *castPtr;

        public:
            SharedPtr()
                : ptr(nullptr, Deleter())
                , castPtr(nullptr)
            {}

            SharedPtr(Base *iptr)
                : ptr(iptr, Deleter())
                , castPtr(dynamic_cast<T *>(iptr))
            {}

            template<typename Other>
            SharedPtr(const SharedPtr<Base, Deleter, Other> &that)
                : ptr(that.ptr)
                , castPtr(dynamic_cast<T *>(that.ptr.get()))
            {}

            template<typename Other>
            SharedPtr(SharedPtr<Base, Deleter, Other> &&that)
                : ptr(std::move(that.ptr))
            {
                castPtr = dynamic_cast<T *>(ptr.get());
            }

            SharedPtr(const std::shared_ptr<Base> &iptr)
                : ptr(iptr)
                , castPtr(dynamic_cast<T *>(iptr.get()))
            {}

            SharedPtr(std::shared_ptr<Base> &&iptr)
                : ptr(std::move(iptr))
            {
                castPtr = dynamic_cast<T *>(ptr.get());
            }

        public:
            T *get() const
            {
                return castPtr;
            }

        public:
            template<typename Other>
            SharedPtr &operator =(const SharedPtr<Base, Deleter, Other> &rhs)
            {
                ptr = rhs.ptr;
                castPtr = dynamic_cast<T *>(ptr.get());
                return *this;
            }

            template<typename Other>
            SharedPtr &operator =(SharedPtr<Base, Deleter, Other> &&rhs)
            {
                ptr = std::move(rhs.ptr);
                castPtr = dynamic_cast<T *>(ptr.get());
                return *this;
            }

            bool operator ==(const SharedPtr &rhs) const
            {
                return castPtr == rhs.castPtr;
            }

            bool operator !=(const SharedPtr &rhs) const
            {
                return castPtr != rhs.castPtr;
            }

            bool operator <(const SharedPtr &rhs) const
            {
                return castPtr < rhs.castPtr;
            }

            bool operator >(const SharedPtr &rhs) const
            {
                return castPtr > rhs.castPtr;
            }

            bool operator <=(const SharedPtr &rhs) const
            {
                return castPtr <= rhs.castPtr;
            }

            bool operator >=(const SharedPtr &rhs) const
            {
                return castPtr >= rhs.castPtr;
            }

            T *operator ->() const
            {
                return castPtr;
            }

            operator bool() const
            {
                return castPtr != nullptr;
            }

            bool operator !() const
            {
                return castPtr == nullptr;
            }

            T &operator *() const
            {
                return *castPtr;
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
            std::weak_ptr<Base> ptr;

        public:
            WeakPtr()
                : ptr()
            {}

            WeakPtr(nullptr_t)
                : ptr()
            {}

            WeakPtr(const std::weak_ptr<Base> &iptr)
                : ptr(iptr)
            {}

            template<typename Other>
            WeakPtr(const SharedPtr<Base, Deleter, Other> &that)
                : ptr(that.ptr)
            {}

            template<typename Other>
            WeakPtr(const WeakPtr<Base, Deleter, SharedPtrReturn, Other> &that)
                : ptr(that.ptr)
            {}

        public:
            SharedPtrReturn lock() const
            {
                return ptr.lock();
            }

        public:
            template<typename Other>
            WeakPtr &operator =(const SharedPtr<Base, Deleter, Other> &rhs)
            {
                ptr = rhs.ptr;
                return *this;
            }

            template<typename Other>
            WeakPtr &operator =(const WeakPtr<Base, Deleter, SharedPtrReturn, Other> &rhs)
            {
                ptr = rhs.ptr;
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

        class ControlManager : public Singleton<ControlManager>
        {
        private:
            std::map<ControlID, ControlWeakPtr<>> controls;

        private:
            ControlID nextID;

        private:
            ControlManager();
            ~ControlManager();

        public:
            ControlID getNextID();
            void add(const ControlID &, const ControlWeakPtr<> &);
            ControlWeakPtr<> find(ControlID);
            void remove(ControlID);

            friend class Singleton<ControlManager>;
        };

        class ShellManager : public Singleton<ShellManager>
        {
        private:
            std::map<HWND, ShellWeakPtr<>> shells;

        private:
            uint32_t nextID;

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

            friend class Singleton<ShellManager>;
        };
    }
}
