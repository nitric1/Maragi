// $Id$

#pragma once

#include "Window.h"
#include "../Singleton.h"

namespace Maragi
{
	namespace UI
	{
		template<typename = Window>
		class WindowPtr;

		template<typename T>
		class WindowPtr
		{
		private:
			std::shared_ptr<Window> ptr;
			T *castPtr;

		public:
			WindowPtr()
				: castPtr(nullptr)
			{
			}

			/*
			WindowPtr(const Window *iptr) // TODO: window by GetDlgItem, ...
			{
				if(iptr == nullptr)
					castPtr = nullptr;
				else
				{
					iptr->id;
				}
			}*/

			WindowPtr(std::shared_ptr<Window> iptr) // TODO: new handler required like egui's new_
				: ptr(iptr)
			{
				castPtr = dynamic_cast<T *>(ptr.get());
			}

			template<typename Other>
			WindowPtr(const WindowPtr<Other> &that)
				: ptr(that.ptr)
			{
				castPtr = dynamic_cast<T *>(ptr.get());
			}

		public:
			bool operator ==(const WindowPtr &rhs)
			{
				return castPtr == rhs.castPtr;
			}

			bool operator !=(const WindowPtr &rhs)
			{
				return castPtr != rhs.castPtr;
			}

			bool operator <(const WindowPtr &rhs)
			{
				return castPtr < rhs.castPtr;
			}

			bool operator >(const WindowPtr &rhs)
			{
				return castPtr > rhs.castPtr;
			}

			bool operator <=(const WindowPtr &rhs)
			{
				return castPtr <= rhs.castPtr;
			}

			bool operator >=(const WindowPtr &rhs)
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

		class WindowManager : public Singleton<WindowManager>
		{
		private:
			std::map<WindowID, WindowPtr<>> windows;
			uintptr_t newID;

		private:
			WindowManager();
			~WindowManager();

		public:
			WindowID getNewVirtualID();

			friend class Singleton<WindowManager>;
		};
	}
}
