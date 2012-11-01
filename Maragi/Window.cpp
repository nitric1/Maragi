// $Id$

#include "Common.h"

#include "Global.h"
#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		const ControlID ControlID::undefined(0);

		class Control::Impl
		{
		private:
			Control *self;

		public:
			explicit Impl(Control *iself)
				: self(iself)
			{
			}

			Control *getParent()
			{
				return self->_parent;
			}

			ControlID getId()
			{
				return self->_id;
			}

			void setId(const ControlID &id)
			{
				::D2D1_SIZE_F;
				self->_id = id;
			}

			Objects::RectangleF getRect()
			{
				return self->_rect;
			}

			void setRect(const Objects::RectangleF &rect)
			{
				self->_rect = rect;
			}
		};

		Control::Control(Control *iparent, ControlID iid)
			: _parent(iparent)
			, _id(iid)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
			id.init(impl.get(), &Impl::getId, &Impl::setId);
			rect.init(impl.get(), &Impl::getRect, &Impl::setRect);
		}

		Control::~Control()
		{
		}

		bool Control::addEventListener(const std::wstring &name, std::shared_ptr<ERDelegate<bool (ControlEventArg)>> listener)
		{
			eventMap.insert(make_pair(name, listener));
			return true;
		}

		bool Control::fireEvent(const std::wstring &name, ControlEventArg e)
		{
			if(eventMap.find(name) == eventMap.end())
				return true;

			auto r = eventMap.equal_range(name);
			for(auto it = r.first; it != r.second; ++ it)
			{
				if(!(*it->second)(e))
					return false;
			}

			return true;
		}

		void Control::draw(Context &context)
		{
		}

		class Shell::Impl
		{
		private:
			Shell *self;

		public:
			explicit Impl(Shell *iself)
				: self(iself)
			{
			}

			Shell *getParent()
			{
				return self->_parent;
			}

			HWND getHwnd()
			{
				return self->_hwnd;
			}

			void setHwnd(HWND hwnd)
			{
				self->_hwnd = hwnd;
			}
		};

		Shell::Shell() // no parent
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
			hwnd.init(impl.get(), &Impl::getHwnd, &Impl::setHwnd);
		}

		Shell::Shell(Shell *iparent) // with parent
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			parent.init(impl.get(), &Impl::getParent);
			hwnd.init(impl.get(), &Impl::getHwnd, &Impl::setHwnd);
		}
	}
}
