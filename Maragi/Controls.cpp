// $Id$

#include "Common.h"

#include "Controls.h"

namespace Maragi
{
	namespace UI
	{
		Label::Label(const ControlID &id)
			: Control(id)
			, color_(Objects::ColorF::Black)
		{
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
			lbl->align(align);
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
				rect(),
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

		const std::wstring &Label::text() const
		{
			return text_;
		}

		void Label::text(const std::wstring &text)
		{
			text_ = text;
			redraw();
		}

		const Objects::ColorF &Label::color() const
		{
			return color_;
		}

		void Label::color(const Objects::ColorF &color)
		{
			color_ = color;
			brush.release();
			redraw();
		}

		uint32_t Label::align() const
		{
			return align_;
		}

		void Label::align(uint32_t align)
		{
			align_ = align;

			switch(align & HORZ_MASK)
			{
			case LEFT:
				format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				break;

			case CENTER:
				format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				break;

			case RIGHT:
				format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
				break;
			}

			switch(align & VERT_MASK)
			{
			case TOP:
				format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
				break;

			case VCENTER:
				format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				break;

			case BOTTOM:
				format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
				break;
			}

			redraw();
		}

		Button::Button(const ControlID &id)
			: Control(id)
			, clicked(false)
		{
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
			Objects::RectangleF rect = this->rect();
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

		const std::wstring &Button::text() const
		{
			return text_;
		}

		void Button::text(const std::wstring &text)
		{
			text_ = text;
			redraw();
		}
	}
}
