// $Id$

#pragma once

#include "../Delegate.h"

namespace Maragi
{
	namespace UI
	{
		template<typename T, typename Operation>
		class Property
		{
		};

#pragma pack(push)
#pragma pack(4)
		struct WindowID
		{
			bool virtualWindow;
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
			Window *parent;
			WindowID id;

		protected:
			explicit Window(Window *);
			virtual ~Window() = 0;

		public:
			virtual void release();
			virtual bool show() = 0;

			Window *getParent();
			const Window *getParent() const;

			WindowID getID() const;

			virtual bool addEventListener(const std::wstring &, std::shared_ptr<ERDelegate<bool (WindowEventArg)>>);

		public:
			;

		protected:
			bool fireEvent(const std::wstring &, WindowEventArg);
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
