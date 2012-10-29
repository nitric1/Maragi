// $Id$

#pragma once

#include "Singleton.h"
#include "Window.h"

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

			/*
			ControlPtr(const Control *iptr) // TODO: window by GetDlgItem, ...
			{
				if(iptr == nullptr)
					castPtr = nullptr;
				else
				{
					iptr->id;
				}
			}*/

			ControlPtr(std::shared_ptr<Control> iptr) // TODO: new handler required like egui's new_
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

		class ControlManager : public Singleton<ControlManager>
		{
		private:
			std::map<ControlID, Control *> windows;

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
	}
}
