// $Id$

#pragma once

#include "../Delegate.h"
#include "../Singleton.h"

#include "Objects.h"

namespace Maragi
{
	namespace UI
	{
		namespace Property
		{
			template<typename Host, typename T>
			class Base
			{
			};

			template<typename Host, typename T>
			class R : public Base<Host, T>
			{
			private:
				std::function<T ()> getter;

			protected:
				void init(std::function<T ()> igetter)
				{
					getter = igetter;
				}

				template<typename Class>
				void init(Class *inst, T (Class::*igetter)())
				{
					getter = std::bind(std::mem_fun(igetter), inst);
				}

			public:
				template<typename Other>
				bool operator ==(const Other &rhs) const
				{
					return getter() == rhs;
				}

				template<typename Other>
				bool operator !=(const Other &rhs) const
				{
					return getter() == rhs;
				}

				T operator ->() const
				{
					return getter();
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

				friend Host;
			};

			template<typename Host, typename T>
			class RWProt : public R<Host, T>
			{
			private:
				std::function<void (const T &)> setter;

			protected:
				void init(std::function<T ()> igetter, std::function<void (const T &)> isetter)
				{
					R<Host, T>::init(igetter);
					setter = isetter;
				}

				template<typename Class>
				void init(Class *inst, T (Class::*igetter)(), void (Class::*isetter)(T))
				{
					R<Host, T>::init(inst, igetter);
					setter = std::bind(std::mem_fun(isetter), inst, std::placeholders::_1);
				}

				template<typename Class>
				void init(Class *inst, T (Class::*igetter)(), void (Class::*isetter)(const T &))
				{
					R<Host, T>::init(inst, igetter);
					setter = std::bind(std::mem_fun(isetter), inst, std::placeholders::_1);
				}

				template<typename Other>
				RWProt &operator =(const Other &val)
				{
					setter(val);
					return *this;
				}

				friend Host;
			};

			template<typename Host, typename T>
			class RW : public RWProt<Host, T>
			{
			public:
				template<typename Other>
				RW &operator =(const Other &val)
				{
					return RWProt<Host, T>::operator =(val);
				}

				friend Host;
			};
		}

		struct WindowID
		{
			static const WindowID undefined;

			uintptr_t id;

			WindowID()
				: id(0)
			{
			}

			explicit WindowID(uintptr_t iid)
				: id(iid)
			{
			}

			WindowID(const WindowID &that)
				: id(that.id)
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

			bool operator <(const WindowID &rhs) const
			{
				return id < rhs.id;
			}

			bool operator >(const WindowID &rhs) const
			{
				return id > rhs.id;
			}

			bool operator <=(const WindowID &rhs) const
			{
				return id <= rhs.id;
			}

			bool operator >=(const WindowID &rhs) const
			{
				return id >= rhs.id;
			}

			WindowID &operator =(const WindowID &rhs)
			{
				id = rhs.id;
				return *this;
			}

			WindowID &operator =(uintptr_t iid)
			{
				id = iid;
				return *this;
			}
		};

		/*
#pragma pack(push)
#pragma pack(4)
		struct __declspec(align(4)) WindowID
		{
			static WindowID undefined;

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

			bool operator <(const WindowID &rhs) const
			{
				if(!virtualWindow && rhs.virtualWindow)
					return true;
				else if(virtualWindow && !rhs.virtualWindow)
					return false;
				else
					return id < rhs.id;
			}

			bool operator >(const WindowID &rhs) const
			{
				if(!virtualWindow && rhs.virtualWindow)
					return false;
				else if(virtualWindow && !rhs.virtualWindow)
					return true;
				else
					return id > rhs.id;
			}

			bool operator <=(const WindowID &rhs) const
			{
				return !(*this > rhs);
			}

			bool operator >=(const WindowID &rhs) const
			{
				return !(*this < rhs);
			}

			WindowID &operator =(const WindowID &rhs)
			{
				virtualWindow = rhs.virtualWindow;
				id = rhs.id;
				return *this;
			}

			WindowID &operator =(uintptr_t iid)
			{
				virtualWindow = true;
				id = iid;
				return *this;
			}

			WindowID &operator =(HWND ihandle)
			{
				virtualWindow = false;
				handle = ihandle;
				return *this;
			}

			WindowID &operator =(nullptr_t)
			{
				*this = undefined;
				return *this;
			}

			operator uintptr_t() const
			{
				return virtualWindow ? id : static_cast<uintptr_t>(-1);
			}

			operator HWND() const
			{
				return virtualWindow ? nullptr : handle;
			}
		};
#pragma pack(pop)
		*/

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
			Objects::Rectangle _rect;

		protected:
			explicit Window(Window *, WindowID);
			virtual ~Window() = 0;

		private: // no implementation
			Window();
			Window(const Window &);

		public:
			virtual void release();
			virtual bool show() = 0;

			virtual bool addEventListener(const std::wstring &, std::shared_ptr<ERDelegate<bool (WindowEventArg)>>);

		protected:
			bool fireEvent(const std::wstring &, WindowEventArg);

		public:
			Property::R<Window, Window *> parent;
			Property::RWProt<Window, WindowID> id;
			Property::RW<Window, Objects::Rectangle> rect;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};

		class WindowManager : public Singleton<WindowManager>
		{
		private:
			std::map<WindowID, Window *> windows;

		private:
			WindowID nextID;

		public:
			WindowManager();

		private:
			~WindowManager();

		private:
			WindowID getNextID();
			void add(Window *);
			Window *find(WindowID);
			void remove(WindowID);

			friend class Window;
			friend class Singleton<WindowManager>;
		};

		/*class Control : public Window
		{
		protected:
			Control();

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};*/
		typedef Window Control;

		class Shell
		{
		private:
			std::multimap<std::wstring, std::shared_ptr<ERDelegate<bool (WindowEventArg)>>> eventMap;

		private:
			Control *child; // Shell handles only one child.

		private:
			Shell *_parent;
			HWND _hwnd;

		protected:
			Shell();
			explicit Shell(Shell *);

		public:
			Property::R<Shell, Shell *> parent;
			Property::RWProt<Shell, HWND> hwnd;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};
	}
}
