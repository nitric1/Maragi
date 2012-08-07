// $Id$

#pragma once

#include "../Delegate.h"

namespace Maragi
{
	namespace UI
	{
		namespace Property
		{
			template<typename T>
			class Base
			{
			};

			template<typename T>
			class R : public Base<T>
			{
			private:
				std::function<T ()> getter;

			public:
				void init(std::function<T ()> igetter)
				{
					getter = igetter;
				}

				template<typename Class>
				void init(Class *inst, T (Class::*igetter)())
				{
					getter = std::bind(std::mem_fun(igetter), inst);
				}

				template<typename Other>
				operator Other()
				{
					return getter();
				}

				template<typename Other>
				operator Other() const
				{
					return getter();
				}
			};

			template<typename T>
			class RW : public R<T>
			{
			private:
				std::function<void (const T &)> setter;

			public:
				void init(std::function<T ()> igetter, std::function<void (const T &)> isetter)
				{
					R<T>::init(igetter);
					setter = isetter;
				}

				template<typename Class>
				void init(Class *inst, T (Class::*igetter)(), void (Class::*isetter)(T))
				{
					R<T>::init(inst, igetter);
					setter = std::bind(std::mem_fun(isetter), inst, std::placeholders::_1);
				}

				template<typename Class>
				void init(Class *inst, T (Class::*igetter)(), void (Class::*isetter)(const T &))
				{
					R<T>::init(inst, igetter);
					setter = std::bind(std::mem_fun(isetter), inst, std::placeholders::_1);
				}

				template<typename Other>
				RW &operator =(const Other &val)
				{
					setter(val);
					return *this;
				}
			};
		}

#pragma pack(push)
#pragma pack(4)
		struct __declspec(align(4)) WindowID
		{
			bool virtualWindow;
			uint8_t padding[3];
			union
			{
				uintptr_t id;
				HWND handle;
			};

			WindowID()
				: virtualWindow(false), id(0)
			{
			}

			WindowID(const WindowID &that)
				: virtualWindow(that.virtualWindow), id(that.id)
			{
			}

			WindowID(uintptr_t iid)
				: virtualWindow(true), id(iid)
			{
			}

			WindowID(HWND ihandle)
				: virtualWindow(false), handle(ihandle)
			{
			}

			operator bool() const
			{
				return id != 0;
			}

			bool operator !() const
			{
				return id == 0;
			}

			bool operator <(const WindowID &obj) const
			{
				if(!virtualWindow && obj.virtualWindow)
					return true;
				else if(virtualWindow && !obj.virtualWindow)
					return false;
				else
					return id < obj.id;
			}

			bool operator >(const WindowID &obj) const
			{
				if(!virtualWindow && obj.virtualWindow)
					return false;
				else if(virtualWindow && !obj.virtualWindow)
					return true;
				else
					return id > obj.id;
			}

			bool operator <=(const WindowID &obj) const
			{
				return !(*this > obj);
			}

			bool operator >=(const WindowID &obj) const
			{
				return !(*this < obj);
			}
		};
#pragma pack(pop)

		class Window;

		struct WindowEventArg
		{
			Window *window;
			unsigned message;
			uintptr_t wParam;
			longptr_t lParam;
		};

		template<typename Func>
		std::shared_ptr<ERDelegate<bool (WindowEventArg)>> delegateWindowEvent(Func fn)
		{
			return delegate<bool (WindowEventArg)>(fn);
		}

		template<typename Class, typename Func>
		std::shared_ptr<ERDelegate<bool (WindowEventArg)>> delegateWindowEvent(Class *p, Func fn)
		{
			return delegate<bool (WindowEventArg)>(p, fn);
		}

		class Window
		{
		private:
			std::multimap<std::wstring, std::shared_ptr<ERDelegate<bool (WindowEventArg)>>> eventMap;

		private:
			Window *_parent;
			WindowID _id;

		protected:
			explicit Window(Window *, WindowID);
			virtual ~Window() = 0;

		public:
			virtual void release();
			virtual bool show() = 0;

			WindowID getID() const;

			virtual bool addEventListener(const std::wstring &, std::shared_ptr<ERDelegate<bool (WindowEventArg)>>);

		protected:
			bool fireEvent(const std::wstring &, WindowEventArg);

		public:
			Property::R<Window *> parent;
			Property::R<WindowID> id;
			Property::RW<int32_t> x, y, width, height;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};

		class HWNDWindow
		{
		protected:
			explicit HWNDWindow();

		private:
			HWND handle;

		public:
			HWND getHandle() const;

		protected:
			void setHandle(HWND);
		};

		class Control : public Window // Do not subclass this class directly
		{
		};

		class ActualControl : public Control, public HWNDWindow
		{
		};

		class VirtualControl : public Control
		{
		private:
			static uintptr_t newId;

		public:
			static WindowID getNewId()
			{
				return WindowID(InterlockedIncrement(&newId));
			}
		};

		class Shell : public Window, public HWNDWindow
		{
		private:
			Control *child; // Shell handles only one child.

		public:
			explicit Shell(Shell *);

			Shell *getParent();
			const Shell *getParent() const;
		};
	}
}
