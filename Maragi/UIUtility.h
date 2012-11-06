// $Id$

#pragma once

#include "Singleton.h"
#include "Primitives.h"
#include "UIForwards.h"

namespace Maragi
{
	namespace UI
	{
		struct ControlPtrDeleter
		{
			void operator ()(Control *) const;
		};

		template<typename = Control>
		class ControlPtr;
		template<typename = Control>
		class ControlWeakPtr;

		template<typename T>
		class ControlPtr
		{
			static_assert(std::is_convertible<T *, Control *>::value, "T must be a derived class from Maragi::UI::Control.");

		private:
			std::shared_ptr<Control> ptr;
			T *castPtr;

		public:
			ControlPtr()
				: ptr(nullptr, ControlPtrDeleter())
				, castPtr(nullptr)
			{}

			ControlPtr(Control *iptr)
				: ptr(iptr, ControlPtrDeleter())
				, castPtr(dynamic_cast<T *>(iptr))
			{}

			template<typename Other>
			ControlPtr(const ControlPtr<Other> &that)
				: ptr(that.ptr)
				, castPtr(dynamic_cast<T *>(that.ptr.get()))
			{}

		private:
			ControlPtr(const std::shared_ptr<Control> &iptr)
				: ptr(iptr)
				, castPtr(dynamic_cast<T *>(iptr.get()))
			{}

		public:
			T *get() const
			{
				return castPtr;
			}

		public:
			template<typename Other>
			ControlPtr &operator =(const ControlPtr<Other> &rhs)
			{
				ptr = rhs.ptr;
				castPtr = dynamic_cast<T *>(ptr.get());
				return *this;
			}

			bool operator ==(const ControlPtr &rhs) const
			{
				return castPtr == rhs.castPtr;
			}

			bool operator !=(const ControlPtr &rhs) const
			{
				return castPtr != rhs.castPtr;
			}

			bool operator <(const ControlPtr &rhs) const
			{
				return castPtr < rhs.castPtr;
			}

			bool operator >(const ControlPtr &rhs) const
			{
				return castPtr > rhs.castPtr;
			}

			bool operator <=(const ControlPtr &rhs) const
			{
				return castPtr <= rhs.castPtr;
			}

			bool operator >=(const ControlPtr &rhs) const
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

			template<typename Other>
			friend class ControlPtr;
			template<typename Other>
			friend class ControlWeakPtr;
		};

		template<typename T>
		class ControlWeakPtr
		{
			static_assert(std::is_convertible<T *, Control *>::value, "T must be a derived class from Maragi::UI::Control.");

		private:
			std::weak_ptr<Control> ptr;

		public:
			ControlWeakPtr()
				: ptr()
			{}

			ControlWeakPtr(nullptr_t)
				: ptr()
			{}

			ControlWeakPtr(const std::weak_ptr<Control> &iptr)
				: ptr(iptr)
			{}

			template<typename Other>
			ControlWeakPtr(const ControlPtr<Other> &that)
				: ptr(that.ptr)
			{}

			template<typename Other>
			ControlWeakPtr(const ControlWeakPtr<Other> &that)
				: ptr(that.ptr)
			{}

		public:
			ControlPtr<T> lock() const
			{
				return ptr.lock();
			}

		public:
			template<typename Other>
			ControlWeakPtr &operator =(const ControlPtr<Other> &rhs)
			{
				ptr = rhs.ptr;
				return *this;
			}

			template<typename Other>
			ControlWeakPtr &operator =(const ControlWeakPtr<Other> &rhs)
			{
				ptr = rhs.ptr;
				return *this;
			}

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
		class ShellPtr
		{
			static_assert(std::is_convertible<T *, Shell *>::value, "T must be a derived class from Maragi::UI::Shell.");

		private:
			std::shared_ptr<Shell> ptr;
			T *castPtr;

		public:
			ShellPtr()
				: ptr(nullptr, ShellPtrDeleter())
				, castPtr(nullptr)
			{}

			ShellPtr(Shell *iptr)
				: ptr(iptr, ShellPtrDeleter())
				, castPtr(dynamic_cast<T *>(iptr))
			{}

			template<typename Other>
			ShellPtr(const ShellPtr<Other> &that)
				: ptr(that.ptr)
				, castPtr(dynamic_cast<T *>(that.ptr.get()))
			{}

		private:
			ShellPtr(const std::shared_ptr<Shell> &iptr)
				: ptr(iptr)
				, castPtr(dynamic_cast<T *>(iptr.get()))
			{}

		public:
			T *get() const
			{
				return castPtr;
			}

		public:
			template<typename Other>
			ShellPtr &operator =(const ShellPtr<Other> &rhs)
			{
				ptr = rhs.ptr;
				castPtr = dynamic_cast<T *>(ptr.get());
				return *this;
			}

			bool operator ==(const ShellPtr &rhs) const
			{
				return castPtr == rhs.castPtr;
			}

			bool operator !=(const ShellPtr &rhs) const
			{
				return castPtr != rhs.castPtr;
			}

			bool operator <(const ShellPtr &rhs) const
			{
				return castPtr < rhs.castPtr;
			}

			bool operator >(const ShellPtr &rhs) const
			{
				return castPtr > rhs.castPtr;
			}

			bool operator <=(const ShellPtr &rhs) const
			{
				return castPtr <= rhs.castPtr;
			}

			bool operator >=(const ShellPtr &rhs) const
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

			template<typename Other>
			friend class ShellPtr;
			template<typename Other>
			friend class ShellWeakPtr;
		};

		template<typename T>
		class ShellWeakPtr
		{
			static_assert(std::is_convertible<T *, Shell *>::value, "T must be a derived class from Maragi::UI::Shell.");

		private:
			std::weak_ptr<Shell> ptr;

		public:
			ShellWeakPtr()
				: ptr()
			{}

			ShellWeakPtr(nullptr_t)
				: ptr()
			{}

			ShellWeakPtr(const std::weak_ptr<Shell> &iptr)
				: ptr(iptr)
			{}

			template<typename Other>
			ShellWeakPtr(const ShellPtr<Other> &that)
				: ptr(that.ptr)
			{}

			template<typename Other>
			ShellWeakPtr(const ShellWeakPtr<Other> &that)
				: ptr(that.ptr)
			{}

		public:
			ShellPtr<T> lock() const
			{
				return ptr.lock();
			}

		public:
			template<typename Other>
			ShellWeakPtr &operator =(const ShellPtr<Other> &rhs)
			{
				ptr = rhs.ptr;
				return *this;
			}

			template<typename Other>
			ShellWeakPtr &operator =(const ShellWeakPtr<Other> &rhs)
			{
				ptr = rhs.ptr;
				return *this;
			}

			template<typename Other>
			friend class ShellWeakPtr;
		};

		class ControlManager : public Singleton<ControlManager>
		{
		private:
			std::map<ControlID, ControlWeakPtr<>> controls;

		private:
			ControlID nextID;

		public:
			ControlManager();

		private:
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

		public:
			ShellManager();

		private:
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
