// $Id$

#include "Common.h"

#include "Layouts.h"

namespace Maragi
{
	namespace UI
	{
		Layout::Layout(Slot *slot, const ControlID &id)
			: Control(slot, id)
		{
		}

		class ShellLayout::Impl
		{
		private:
			ShellLayout *self;

		public:
			explicit Impl(ShellLayout *iself)
				: self(iself)
			{}

			ShellWeakPtr<> getShell()
			{
				return self->shell_;
			}

			ControlWeakPtr<> getChild()
			{
				return self->child_;
			}
		};

		ShellLayout::ShellLayout(const ShellWeakPtr<> &ishell, const ControlID &id)
			: Layout(nullptr, id)
			, shell_(ishell)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			shell.init(impl.get(), &Impl::getShell);
			child.init(impl.get(), &Impl::getChild);
		}

		ControlPtr<ShellLayout> ShellLayout::create(
			const ShellWeakPtr<> &shell
			)
		{
			return new ShellLayout(shell, ControlManager::instance().getNextID());
		}

		ShellLayout::~ShellLayout()
		{
		}

		void ShellLayout::createDrawingResources(Drawing::Context &ctx)
		{
			// TODO: propagate
		}

		void ShellLayout::discardDrawingResources(Drawing::Context &ctx)
		{
			// TODO: propagate
		}

		void ShellLayout::draw(Drawing::Context &ctx)
		{
			// TODO: propagate
		}
	}
}
