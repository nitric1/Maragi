// $Id$

#pragma once

#include "Delegate.h"
#include "Objects.h"
#include "Singleton.h"

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

		struct ControlEventArg
		{
			Control *window;
		};

		template<typename Func>
		std::shared_ptr<ERDelegate<bool (ControlEventArg)>> delegateWindowEvent(Func fn)
		{
			return delegate<bool (ControlEventArg)>(fn);
		}

		template<typename Class, typename Func>
		std::shared_ptr<ERDelegate<bool (ControlEventArg)>> delegateWindowEvent(Class *p, Func fn)
		{
			return delegate<bool (ControlEventArg)>(p, fn);
		}

		class Control
		{
		private:
			std::multimap<std::wstring, std::shared_ptr<ERDelegate<bool (ControlEventArg)>>> eventMap;

		private:
			Control *_parent;
			ControlID _id;
			Objects::Rectangle _rect;

		protected:
			explicit Control(Control *, ControlID);
			virtual ~Control() = 0;

		private: // no implementation
			Control();
			Control(const Control &);

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
			Property::RW<Control, Objects::Rectangle> rect;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};

		class Shell
		{
		private:
			std::multimap<std::wstring, std::shared_ptr<ERDelegate<bool (ControlEventArg)>>> eventMap;

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
