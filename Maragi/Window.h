// $Id$

#pragma once

#include "Delegate.h"
#include "Drawing.h"
#include "Global.h"
#include "Objects.h"
#include "Primitives.h"
#include "Singleton.h"
#include "UIUtility.h"

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
				T get() const
				{
					return getter();
				}

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
			class RW : public R<Host, T>
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

			public:
				template<typename Other>
				RW &operator =(const Other &val)
				{
					setter(val);
					return *this;
				}

				friend Host;
			};
		}

		namespace PropertyValue
		{
			template<typename Host, typename T>
			class Base
			{
			protected:
				T val;

			protected:
				Base()
					: val()
				{}
			};

			// TODO: wrapper
			template<typename Host, typename T>
			class R : public Base<Host, T>
			{
			protected:
				R()
					: Base()
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
				const T &get() const
				{
					return val;
				}

				template<typename Other>
				bool operator ==(const Other &rhs) const
				{
					return val == rhs;
				}

				template<typename Other>
				bool operator !=(const Other &rhs) const
				{
					return val != rhs;
				}

				operator const T &() const
				{
					return val;
				}

				template<typename Other>
				operator const Other &() const
				{
					return val;
				}

				friend Host;
			};
		}

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

		class Slot
		{
		private:
			ControlWeakPtr<> parent_;
			ControlWeakPtr<> child_;

		public:
			Slot();
			explicit Slot(const ControlWeakPtr<> &);
			virtual ~Slot();

		private:
			Slot(const Slot &); // = delete;

		public:
			virtual bool attach(const ControlWeakPtr<> &);
			virtual ControlWeakPtr<> detach();

		public:
			Property::RW<Slot, ControlWeakPtr<>> parent; // TODO: should be fixed
			Property::R<Slot, ControlWeakPtr<>> child;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;

			friend class Control;
		};

		template<typename Arg>
		class Event
		{
		private:
			boost::signals2::signal<void (const Arg &)> sig;

		public:
			Event() {}
			~Event() {}

		private:
			Event(const Event &); // = delete;

		public:
			template<typename Func>
			boost::signals2::connection connect(Func fn, bool prior = false)
			{
				if(prior)
					return sig.connect(0, fn);
				return sig.connect(fn);
			}

			template<typename FunctionType>
			boost::signals2::connection connect(const ERDelegateWrapper<FunctionType> &dg, bool prior = false)
			{
				if(prior)
					return sig.connect(0, dg);
				return sig.connect(dg);
			}

			template<typename FunctionType>
			boost::signals2::connection connect(const std::shared_ptr<ERDelegate<FunctionType>> &dg, bool prior = false)
			{
				if(prior)
					return sig.connect(0, ERDelegateWrapper<FunctionType>(dg));
				return sig.connect(ERDelegateWrapper<FunctionType>(dg));
			}

			void operator ()(const Arg &arg)
			{
				sig(arg);
				// TODO: check propagatable
			}

		public:
			template<typename Func>
			boost::signals2::connection operator +=(Func rhs)
			{
				return sig.connect(rhs);
			}

			template<typename FunctionType>
			boost::signals2::connection operator +=(const ERDelegateWrapper<FunctionType> &rhs)
			{
				return sig.connect(rhs);
			}

			template<typename FunctionType>
			boost::signals2::connection connect(const std::shared_ptr<ERDelegate<FunctionType>> &rhs)
			{
				return sig.connect(ERDelegateWrapper<FunctionType>(rhs));
			}
		};

		struct ControlEventArg
		{
			ControlWeakPtr<> control;
			boost::posix_time::ptime time;

			// raw
			uint32_t rawMessage;
			uintptr_t wParam;
			longptr_t lParam;

			// mouse
			uint32_t buttonNum; // 1 (left), 2 (right), 3 (middle), 4, 5, ...
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

			/*void stopPropagation() const
			{
				propagatable = false;
			}

		private:
			mutable bool propagatable;*/

			template<typename>
			friend class Event;
		};

		template<typename Func>
		ERDelegateWrapper<void (const ControlEventArg &)> delegateControlEvent(Func fn)
		{
			return delegate<void (const ControlEventArg &)>(fn);
		}

		template<typename Class, typename Func>
		ERDelegateWrapper<void (const ControlEventArg &)> delegateControlEvent(Class *p, Func fn)
		{
			return delegate<void (const ControlEventArg &)>(p, fn);
		}

		typedef Event<ControlEventArg> ControlEvent;

		class Control : public std::enable_shared_from_this<Control>
		{
		private:
			Slot *parent_;
			ControlID id_;
			Objects::RectangleF rect_;

		protected:
			Control(const ControlID &);
			virtual ~Control() = 0;

		private: // no implementation
			Control(); // = delete;
			Control(const Control &); // = delete;

		protected:
			ControlPtr<> sharedFromThis();

		public:
			virtual void createDrawingResources(Drawing::Context &);
			virtual void discardDrawingResources(Drawing::Context &);
			virtual void draw(Drawing::Context &) = 0;
			virtual Objects::SizeF computeSize() = 0;
			virtual ControlWeakPtr<> findByPoint(const Objects::PointF &);

		public: // internal event handlers
			virtual void onResizeInternal(const Objects::RectangleF &);
			virtual bool onSetCursorInternal();

		public: // external event handlers
			ControlEvent onMouseMove;
			ControlEvent onMouseButtonDown;
			ControlEvent onMouseButtonDoubleClick;
			ControlEvent onMouseButtonUp;

		public:
			Property::R<Control, Slot *> parent;
			Property::R<Control, ControlID> id;
			Property::RW<Control, Objects::RectangleF> rect;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;

			friend class Slot;
			friend class Shell;
			friend struct ControlPtrDeleter;
		};

		struct WindowEventArg
		{
			ShellWeakPtr<> shell;
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

		class Shell : public std::enable_shared_from_this<Shell>
		{
		private:
			std::multimap<std::wstring, std::shared_ptr<ERDelegate<bool (WindowEventArg &)>>> eventMap;

		private:
			ShellWeakPtr<> parent_;

		protected:
			HWND hwnd_;

		protected:
			Shell();
			explicit Shell(const ShellWeakPtr<> &);
			virtual ~Shell() = 0;

		private: // no implementation
			Shell(const Shell &); // = delete;

		public:
			virtual bool show() = 0;

		protected:
			ShellPtr<> sharedFromThis();

		public:
			Property::R<Shell, ShellWeakPtr<>> parent;
			Property::R<Shell, HWND> hwnd;
			Property::R<Shell, Objects::SizeI> clientSize;

		private:
			virtual longptr_t procMessage(HWND, uint32_t, uintptr_t, longptr_t)
			{
				return 0;
			}

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;

			friend struct ShellPtrDeleter;
			friend class ShellManager;
		};
	}
}
