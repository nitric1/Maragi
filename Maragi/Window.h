// $Id$

#pragma once

#include "Delegate.h"
#include "Objects.h"
#include "Singleton.h"

namespace Maragi
{
	namespace UI
	{
		/*
		UI::ShellPtr<UI::FrameWindow> frm = UI::FrameWindow::create("Hello World", UI::Icon::fromResource(...), ...);
		UI::ControlPtr<UI::GridLayout> layout = UI::GridLayout::create(frm, 1, 2);
		UI::ControlPtr<UI::Button> button = UI::Button::create("Button Text");
		button.onClick = delegate(this, &onButtonClick);
		frm->show();
		*/

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
					return getter() != rhs;
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
			class RV /* readonly having value */ : public Base<Host, T>
			{
			private:
				T val;

			private:
				RV()
					: val()
				{}

			protected:
				void init(const T &ival)
				{
					val = ival;
				}

				void init(T &&ival)
				{
					val = std::forward<T>(ival);
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
					return getter() != rhs;
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

		struct ControlID
		{
			static const ControlID undefined;

			uintptr_t id;

			ControlID()
				: id(0)
			{
			}

			explicit ControlID(uintptr_t iid)
				: id(iid)
			{
			}

			ControlID(const ControlID &that)
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

			bool operator <(const ControlID &rhs) const
			{
				return id < rhs.id;
			}

			bool operator >(const ControlID &rhs) const
			{
				return id > rhs.id;
			}

			bool operator <=(const ControlID &rhs) const
			{
				return id <= rhs.id;
			}

			bool operator >=(const ControlID &rhs) const
			{
				return id >= rhs.id;
			}

			ControlID &operator =(const ControlID &rhs)
			{
				id = rhs.id;
				return *this;
			}

			ControlID &operator =(uintptr_t iid)
			{
				id = iid;
				return *this;
			}
		};

		class Control;

		class Context
		{
		};

		namespace Message
		{
			enum
			{
				KeyDown = WM_KEYDOWN,
				KeyUp = WM_KEYUP,
				KeyChar = WM_CHAR,
				// TODO: IME messages
				KeyImeStartComposition = WM_IME_STARTCOMPOSITION,
				KeyImeEndComposition = WM_IME_ENDCOMPOSITION,
				KeyImeComposition = WM_IME_COMPOSITION,

				MouseMove = WM_MOUSEMOVE,
				MouseLButtonDown = WM_LBUTTONDOWN,
				MouseLButtonUp = WM_LBUTTONUP,
				MouseLButtonDoubleClick = WM_LBUTTONDBLCLK,
				MouseRButtonDown = WM_RBUTTONDOWN,
				MouseRButtonUp = WM_RBUTTONUP,
				MouseRButtonDoubleClick = WM_RBUTTONDBLCLK,
				MouseMButtonDown = WM_MBUTTONDOWN,
				MouseMButtonUp = WM_MBUTTONUP,
				MouseMButtonDoubleClick = WM_MBUTTONDBLCLK,
				MouseXButtonDown = WM_XBUTTONDOWN,
				MouseXButtonUp = WM_XBUTTONUP,
				MouseXButtonDoubleClick = WM_XBUTTONDBLCLK,
				MouseWheel = WM_MOUSEWHEEL,
				MouseWheelHorizontal = WM_MOUSEHWHEEL,
			};
		}

		struct ControlEventArg
		{
			Control *control;
			uint32_t message;
			boost::posix_time::ptime time;

			// raw
			uint32_t rawMessage;
			uintptr_t wParam;
			longptr_t lParam;

			// mouse
			uint32_t buttonNum;
			Objects::PointF controlPoint;
			Objects::PointF shellClientPoint;
			Objects::PointF screenPoint;
			float wheelAmount;

			// key
			bool altKey;
			bool ctrlKey;
			bool shiftKey;
			char keyCode; // Not in Char, ImeChar?
			wchar_t charCode; // Char, ImeChar?
			uint32_t repeated;
		};

		template<typename Func>
		std::shared_ptr<ERDelegate<bool (ControlEventArg &)>> delegateControlEvent(Func fn)
		{
			return delegate<bool (ControlEventArg)>(fn);
		}

		template<typename Class, typename Func>
		std::shared_ptr<ERDelegate<bool (ControlEventArg &)>> delegateControlEvent(Class *p, Func fn)
		{
			return delegate<bool (ControlEventArg &)>(p, fn);
		}

		class Control
		{
		private:
			std::multimap<std::wstring, std::shared_ptr<ERDelegate<bool (ControlEventArg)>>> eventMap;

		private:
			Control *_parent;
			ControlID _id;
			Objects::RectangleF _rect;

		protected:
			Control(Control *, ControlID);
			// explicit Control(const ControlCreateParams &);
			virtual ~Control() = 0;

		private: // no implementation
			Control(); // = delete;
			Control(const Control &); // = delete;

		public:
			// virtual void release();
			virtual bool show() = 0;

			virtual bool addEventListener(const std::wstring &, std::shared_ptr<ERDelegate<bool (ControlEventArg)>>);

		protected:
			bool fireEvent(const std::wstring &, ControlEventArg);

		public:
			virtual void draw(Context &);

		public:
			Property::R<Control, Control *> parent;
			Property::RWProt<Control, ControlID> id;
			Property::RW<Control, Objects::RectangleF> rect;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;

			friend class Shell;
		};

		struct WindowEventArg
		{
			Shell *shell;
			uint32_t message;
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

		private: // no implementation
			Shell(const Shell &); // = delete;

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
