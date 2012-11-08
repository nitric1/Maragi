// $Id$

#include "Common.h"

#include "Layouts.h"

namespace Maragi
{
	namespace UI
	{
		Layout::Layout(const ControlID &id)
			: Control(id)
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

			Slot *getSlot()
			{
				return &self->slot_;
			}
		};

		ShellLayout::ShellLayout(const ShellWeakPtr<> &ishell, const ControlID &id)
			: Layout(id)
			, shell_(ishell)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			shell.init(impl.get(), &Impl::getShell);
			slot.init(impl.get(), &Impl::getSlot);
		}

		ControlPtr<ShellLayout> ShellLayout::create(
			const ShellWeakPtr<> &shell
			)
		{
			ControlPtr<ShellLayout> layout(new ShellLayout(shell, ControlManager::instance().getNextID()));
			layout->slot_.parent = layout;
			return layout;
		}

		ShellLayout::~ShellLayout()
		{
		}

		void ShellLayout::createDrawingResources(Drawing::Context &ctx)
		{
			ControlPtr<> lchild = slot_.child.get().lock();
			if(lchild)
				lchild->createDrawingResources(ctx);
		}

		void ShellLayout::discardDrawingResources(Drawing::Context &ctx)
		{
			ControlPtr<> lchild = slot_.child.get().lock();
			if(lchild)
				lchild->discardDrawingResources(ctx);
		}

		void ShellLayout::draw(Drawing::Context &ctx)
		{
			ControlPtr<> lchild = slot_.child.get().lock();
			if(lchild)
				lchild->draw(ctx);
		}

		Objects::SizeF ShellLayout::computeSize()
		{
			ControlPtr<> lchild = slot_.child.get().lock();
			if(lchild)
				return lchild->computeSize();
			return Objects::SizeF();
		}

		void ShellLayout::onResizeInternal(const Objects::RectangleF &rect)
		{
			ControlPtr<> lchild = slot_.child.get().lock();
			lchild->rect = rect;
		}
	}
}
