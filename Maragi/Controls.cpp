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

		void Label::draw(Drawing::Context &ctx)
		{
			;
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
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			text.init(impl.get(), &Impl::getText, &Impl::setText);

			onMouseButtonDown.connect(delegateControlEvent(this, &Button::onMouseButtonDownImpl));
			onMouseButtonUp.connect(delegateControlEvent(this, &Button::onMouseButtonDownImpl));
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
			ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGray), &brush);
		}

		void Button::discardDrawingResources(Drawing::Context &ctx)
		{
			brush.release();
		}

		void Button::draw(Drawing::Context &ctx)
		{
			Objects::RectangleF rect = this->rect.get();
			rect.left += 50.0f;
			rect.top += 50.0f;
			rect.right -= 50.0f;
			rect.bottom -= 50.0f;
			ctx->FillRoundedRectangle(D2D1::RoundedRect(rect, 4.0f, 4.0f), brush);
		}

		Objects::SizeF Button::computeSize()
		{
			// TODO: calculate
			return Objects::SizeF(64.0f, 64.0f);
		}

		void Button::onMouseButtonDownImpl(const ControlEventArg &arg)
		{
		}

		void Button::onMouseButtonUpImpl(const ControlEventArg &arg)
		{
		}
	}
}
