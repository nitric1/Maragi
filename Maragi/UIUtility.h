// $Id$

#pragma once

#include "Singleton.h"
#include "Primitives.h"
#include "UIForwards.h"

namespace Maragi
{
	namespace UI
	{
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
				: castPtr(nullptr)
			{
			}

			ControlPtr(Control *iptr)
			{
				if(iptr == nullptr)
					castPtr = nullptr;
				else
				{
					castPtr = dynamic_cast<T *>(iptr);
					ptr = std::shared_ptr<Control>(iptr);
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
			bool operator ==(const ControlPtr &rhs)
			{
				return castPtr == rhs.castPtr;
			}

			bool operator !=(const ControlPtr &rhs)
			{
				return castPtr != rhs.castPtr;
			}

			bool operator <(const ControlPtr &rhs)
			{
				return castPtr < rhs.castPtr;
			}

			bool operator >(const ControlPtr &rhs)
			{
				return castPtr > rhs.castPtr;
			}

			bool operator <=(const ControlPtr &rhs)
			{
				return castPtr <= rhs.castPtr;
			}

			bool operator >=(const ControlPtr &rhs)
			{
				return castPtr >= rhs.castPtr;
			}

			T *operator ->()
			{
				return castPtr;
			}

			const T *operator ->() const
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
		};

		template<typename = Shell>
		class ShellPtr;

		template<typename T>
		class ShellPtr
		{
		};

		class ControlManager : public Singleton<ControlManager>
		{
		private:
			std::map<ControlID, Control *> controls;

		private:
			ControlID nextID;

		public:
			ControlManager();

		private:
			~ControlManager();

		private:
			ControlID getNextID();
			void add(Control *);
			Control *find(ControlID);
			void remove(ControlID);

			friend class Control;
			friend class Singleton<ControlManager>;
		};

		class ShellManager : public Singleton<ShellManager>
		{
		private:
			std::map<HWND, Shell *> shells;

		public:
			ShellManager();

		private:
			~ShellManager();

		private:

			friend class Shell;
			friend class Singleton<ShellManager>;
		};

		template<typename T>
		class ResourcePtr : public std::shared_ptr<T>
		{
		private:
			struct Deleter
			{
				void operator ()(T *ptr) const
				{
					ptr->release();
				}
			};

		public:
			ResourcePtr()
				: std::shared_ptr<T>()
			{}

			explicit ResourcePtr(T *ptr)
				: std::shared_ptr<T>(ptr)
			{}

			ResourcePtr(nullptr_t)
				: std::shared_ptr<T>(nullptr)
			{}

			template<typename Other>
			ResourcePtr(const ResourcePtr<Other> &that)
				: std::shared_ptr<T>(that)
			{}

			template<typename Other>
			ResourcePtr(ResourcePtr<Other> &&that)
				: std::shared_ptr<T>(that)
			{}
		};
	}
}
