// $Id$

#pragma once

namespace Maragi
{
	namespace UI
	{
		class Window;

		struct WindowEventArg
		{
			Window *window;
			unsigned message;
			WPARAM wParam;
			LPARAM lParam;
		};

		// TODO: General-purpose delegate
		class WindowEventCallback
		{
		public:
			virtual bool run(WindowEventArg e) = 0;
			bool operator ()(WindowEventArg e) { return run(e); }
		};

		template<typename Func>
		class WindowEventFunctionCallback : public WindowEventCallback
		{
		private:
			Func fn;

		private:
			explicit WindowEventFunctionCallback(Func ifn) : fn(ifn) {}

		public:
			virtual bool run(WindowEventArg e) { return fn(e); }

			template<typename Func>
			friend std::shared_ptr<WindowEventFunctionCallback<Func>> delegateWindowEvent(Func);
		};

		template<typename Class, typename Func>
		class WindowEventMemberFunctionCallback : public WindowEventCallback
		{
		private:
			Class *p;
			Func fn;

		private:
			explicit WindowEventMemberFunctionCallback(Class *ip, Func ifn) : p(ip), fn(ifn) {}

		public:
			virtual bool run(WindowEventArg e) { return (p->*fn)(e); }

			template<typename Class, typename Func>
			friend std::shared_ptr<WindowEventMemberFunctionCallback<Class, Func>> delegateWindowEvent(Class *, Func);
		};

		template<typename Func>
		std::shared_ptr<WindowEventFunctionCallback<Func>> delegateWindowEvent(Func fn)
		{
			return decltype(delegateWindowEvent(fn))(new WindowEventFunctionCallback<Func>(fn));
		}

		template<typename Class, typename Func>
		std::shared_ptr<WindowEventMemberFunctionCallback<Class, Func>> delegateWindowEvent(Class *p, Func fn)
		{
			return decltype(delegateWindowEvent(p, fn))(new WindowEventMemberFunctionCallback<Class, Func>(p, fn));
		}

		class Window
		{
		private:
			std::multimap<std::wstring, std::shared_ptr<WindowEventCallback>> eventMap;

		private:
			HWND window;
			int showCommand;
			Window *parent;

		protected:
			explicit Window(HWND = nullptr);
			virtual ~Window() = 0;

		public:
			virtual int getShowStatus() const;
			virtual void setShowStatus(int);

			virtual bool show() = 0;

			virtual HWND getWindow() const;
			virtual void setWindow(HWND);
			Window *getParent();
			const Window *getParent() const;
			void setParent(Window *);
			virtual bool showWithParent(Window *);
			virtual bool addEventListener(const std::wstring &, std::shared_ptr<WindowEventCallback>);

		protected:
			bool fireEvent(const std::wstring &, WindowEventArg);
		};
	}
}
