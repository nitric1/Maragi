// $Id$

#include "Common.h"

#include "Controls.h"

namespace Maragi
{
	namespace UI
	{
		class Label::Impl
		{
		private:
			Label *self;

		public:
			explicit Impl(Label *iself)
				: self(iself)
			{}

			std::wstring getText()
			{
				return self->text_;
			}

			void setText(const std::wstring &text)
			{
				self->text_ = text;
				self->redraw();
			}
		};

		Label::Label(const ControlID &id)
			: Control(id)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			text.init(impl.get(), &Impl::getText, &Impl::setText);
		}

		Label::~Label()
		{}

		ControlPtr<Label> Label::create(
			const std::wstring &text
			)
		{
			ControlPtr<Label> lbl(new Label(ControlManager::instance().getNextID()));
			lbl->text_ = text;
			return lbl;
		}

		void Label::createDrawingResources(Drawing::Context &ctx)
		{
		}

		void Label::discardDrawingResources(Drawing::Context &ctx)
		{
		}

		void Label::draw(Drawing::Context &ctx)
		{
		}

		Objects::SizeF Label::computeSize()
		{
			// TODO: calculate
			return Objects::SizeF(64.0f, 64.0f);
		}

		class Button::Impl
		{
		private:
			Button *self;

		public:
			explicit Impl(Button *iself)
				: self(iself)
			{}

			std::wstring getText()
			{
				return self->text_;
			}

			void setText(const std::wstring &text)
			{
				self->text_ = text;
			}
		};

		Button::Button(const ControlID &id)
			: Control(id)
			, clicked(false)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			text.init(impl.get(), &Impl::getText, &Impl::setText);

			onMouseButtonDown += delegateControlEvent(this, &Button::onMouseButtonDownImpl);
			onMouseButtonDoubleClick += delegateControlEvent(this, &Button::onMouseButtonDownImpl);
			onMouseButtonUp += delegateControlEvent(this, &Button::onMouseButtonUpImpl);
		}

		Button::~Button()
		{}

		ControlPtr<Button> Button::create(
			const std::wstring &text
			)
		{
			ControlPtr<Button> btn(new Button(ControlManager::instance().getNextID()));
			btn->text_ = text;
			return btn;
		}

		void Button::createDrawingResources(Drawing::Context &ctx)
		{
			ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGray), &brushUp);
			ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &brushDown);
		}

		void Button::discardDrawingResources(Drawing::Context &ctx)
		{
			brushUp.release();
			brushDown.release();
		}

		void Button::draw(Drawing::Context &ctx)
		{
			Objects::RectangleF rect = this->rect.get();
			rect.left += 50.0f;
			rect.top += 50.0f;
			rect.right -= 50.0f;
			rect.bottom -= 50.0f;
			if(clicked)
				ctx->FillRoundedRectangle(D2D1::RoundedRect(rect, 4.0f, 4.0f), brushDown);
			else
				ctx->FillRoundedRectangle(D2D1::RoundedRect(rect, 4.0f, 4.0f), brushUp);
		}

		Objects::SizeF Button::computeSize()
		{
			// TODO: calculate
			return Objects::SizeF(64.0f, 64.0f);
		}

		void Button::onMouseButtonDownImpl(const ControlEventArg &arg)
		{
			if(arg.buttonNum == 1)
			{
				clicked = true;
				redraw();
			}
		}

		void Button::onMouseButtonUpImpl(const ControlEventArg &arg)
		{
			if(arg.buttonNum == 1)
			{
				onClick(arg);
				clicked = false;
				redraw();
			}
		}
	}
}
