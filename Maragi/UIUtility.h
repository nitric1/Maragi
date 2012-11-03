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

		template<typename T>
		class ControlPtr
		{
		private:
			std::shared_ptr<Control> ptr;
			T *castPtr;

		public:
			ControlPtr()
				: ptr(nullptr, ControlPtrDeleter())
				, castPtr(nullptr)
			{
			}

			ControlPtr(Control *iptr)
				: castPtr(nullptr)
			{
				if(iptr != nullptr)
				{
					castPtr = dynamic_cast<T *>(iptr);
					ptr = std::shared_ptr<Control>(iptr, ControlPtrDeleter());
				}
			}

			ControlPtr(std::shared_ptr<Control> iptr)
				: ptr(iptr)
			{
				castPtr = dynamic_cast<T *>(ptr.get());
			}

			template<typename Other>
			ControlPtr(const ControlPtr<Other> &that)
				: ptr(that.ptr)
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
		};

		struct ShellPtrDeleter
		{
			void operator ()(Shell *) const;
		};

		template<typename = Shell>
		class ShellPtr;

		template<typename T>
		class ShellPtr
		{
		private:
			std::shared_ptr<Shell> ptr;
			T *castPtr;

		public:
			ShellPtr()
				: ptr(nullptr, ShellPtrDeleter())
				, castPtr(nullptr)
			{
			}

			ShellPtr(Shell *iptr)
			{
				if(iptr == nullptr)
					castPtr = nullptr;
				else
				{
					castPtr = dynamic_cast<T *>(iptr);
					ptr = std::shared_ptr<Shell>(iptr, ShellPtrDeleter());
				}
			}

			ShellPtr(std::shared_ptr<Shell> iptr)
				: ptr(iptr)
			{
				castPtr = dynamic_cast<T *>(ptr.get());
			}

			template<typename Other>
			ShellPtr(const ShellPtr<Other> &that)
				: ptr(that.ptr)
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
		};

		class ControlManager : public Singleton<ControlManager>
		{
		private:
			std::map<ControlID, ControlPtr<>> controls;

		private:
			ControlID nextID;

		public:
			ControlManager();

		private:
			~ControlManager();

		public:
			ControlID getNextID();
			void add(const ControlID &, const ControlPtr<> &);
			ControlPtr<> find(ControlID);
			void remove(ControlID);

			friend class Singleton<ControlManager>;
		};

		class ShellManager : public Singleton<ShellManager>
		{
		private:
			std::map<HWND, ShellPtr<>> shells;

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
			void add(HWND, const ShellPtr<> &);
			ShellPtr<> find(HWND);
			void remove(HWND);

			friend class Singleton<ShellManager>;
		};
	}
}
