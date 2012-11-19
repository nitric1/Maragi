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

			Objects::ColorF getColor()
			{
				return self->color_;
			}

			void setColor(const Objects::ColorF &color)
			{
				self->color_ = color;
				self->brush.release();
				self->redraw();
			}

			uint32_t getAlign()
			{
				return self->align_;
			}

			void setAlign(uint32_t align)
			{
				self->align_ = align;

				switch(align & HORZ_MASK)
				{
				case LEFT:
					self->format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
					break;

				case CENTER:
					self->format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
					break;

				case RIGHT:
					self->format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
					break;
				}

				switch(align & VERT_MASK)
				{
				case TOP:
					self->format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
					break;

				case VCENTER:
					self->format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
					break;

				case BOTTOM:
					self->format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
					break;
				}

				self->redraw();
			}
		};

		Label::Label(const ControlID &id)
			: Control(id)
			, color_(Objects::ColorF::Black)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			text.init(impl.get(), &Impl::getText, &Impl::setText);
			color.init(impl.get(), &Impl::getColor, &Impl::setColor);
			align.init(impl.get(), &Impl::getAlign, &Impl::setAlign);

			format = Drawing::FontFactory::instance().createFont(12.0f);

			const auto &dwfac = Drawing::D2DFactory::instance().getDWriteFactory();
			ComPtr<IDWriteRenderingParams> renderParamsTemp;
			dwfac->CreateRenderingParams(&renderParamsTemp);
			dwfac->CreateCustomRenderingParams(
				renderParamsTemp->GetGamma(),
				renderParamsTemp->GetEnhancedContrast(),
				renderParamsTemp->GetClearTypeLevel(),
				renderParamsTemp->GetPixelGeometry(),
				DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC,
				&renderParams
				);
		}

		Label::~Label()
		{}

		ControlPtr<Label> Label::create(
			const std::wstring &text,
			const Objects::ColorF &color,
			uint32_t align
			)
		{
			ControlPtr<Label> lbl(new Label(ControlManager::instance().getNextID()));
			lbl->text_ = text;
			lbl->color_ = color;
			lbl->align = align;
			return lbl;
		}

		void Label::createDrawingResources(Drawing::Context &ctx)
		{
		}

		void Label::discardDrawingResources(Drawing::Context &ctx)
		{
			brush.release();
		}

		void Label::draw(Drawing::Context &ctx)
		{
			if(!brush)
			{
				HRESULT hr;
				hr = ctx->CreateSolidColorBrush(color_, &brush);
				if(FAILED(hr))
					throw(UIException("CreateSolidColorBrush failed in Label::draw."));
			}

			ComPtr<IDWriteRenderingParams> originalRenderParams;
			ctx->GetTextRenderingParams(&originalRenderParams);
			ctx->SetTextRenderingParams(renderParams);
			ctx->DrawTextW(
				text_.c_str(),
				static_cast<unsigned>(text_.size()),
				format,
				rect.get(),
				brush,
				D2D1_DRAW_TEXT_OPTIONS_NONE,
				DWRITE_MEASURING_MODE_GDI_CLASSIC
				);
			ctx->SetTextRenderingParams(originalRenderParams);
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

			onMouseButtonDown += delegate(this, &Button::onMouseButtonDownImpl);
			onMouseButtonDoubleClick += delegate(this, &Button::onMouseButtonDownImpl);
			onMouseButtonUp += delegate(this, &Button::onMouseButtonUpImpl);
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
			// TODO: neat rendering
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
				clicked = false;
				redraw();
				onClick(arg);
			}
		}
	}
}
