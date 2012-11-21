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
			format = Drawing::FontFactory::instance().createFont(16.0f); // TODO: font size customization?

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
			, hovered(false)
		{
			onMouseButtonDown += delegate(this, &Button::onMouseButtonDownImpl);
			onMouseButtonDoubleClick += delegate(this, &Button::onMouseButtonDownImpl);
			onMouseButtonUp += delegate(this, &Button::onMouseButtonUpImpl);

			onMouseOver += delegate([this](const ControlEventArg &arg)
			{
				hovered = true;
				redraw();
			});

			onMouseOut += delegate([this](const ControlEventArg &arg)
			{
				hovered = false;
				redraw();
			});
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
			ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Silver), &brushHovered);
		}

		void Button::discardDrawingResources(Drawing::Context &ctx)
		{
			brushUp.release();
			brushDown.release();
			brushHovered.release();
		}

		void Button::draw(Drawing::Context &ctx)
		{
			Objects::RectangleF rect = this->rect();
			if(clicked)
				ctx->FillRoundedRectangle(D2D1::RoundedRect(rect, 4.0f, 4.0f), brushDown);
			else if(hovered)
				ctx->FillRoundedRectangle(D2D1::RoundedRect(rect, 4.0f, 4.0f), brushHovered);
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

		Edit::Edit(const ControlID &id)
			: Control(id)
			, colorText_(Objects::ColorF::Black)
			, colorPlaceholder_(Objects::ColorF::DarkGray)
			, colorBackground_(Objects::ColorF::Black)
		{
			formatText = Drawing::FontFactory::instance().createFont(16.0f); // TODO: font size customization?
			formatPlaceholder = Drawing::FontFactory::instance().createFont(
				16.0f,
				DWRITE_FONT_WEIGHT_BOLD
				);

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

		Edit::~Edit()
		{}

		ControlPtr<Edit> Edit::create(
			const std::wstring &placeholder,
			const Objects::ColorF &colorText,
			const Objects::ColorF &colorPlaceholder,
			const Objects::ColorF &colorBackground,
			const std::wstring &text,
			size_t selStart,
			size_t selEnd
			)
		{
			ControlPtr<Edit> edit(new Edit(ControlManager::instance().getNextID()));
			edit->placeholder_ = placeholder;
			edit->colorText_ = colorText;
			edit->colorPlaceholder_ = colorPlaceholder;
			edit->colorBackground_ = colorBackground;
			edit->text_ = text;
			edit->selStart_ = selStart;
			edit->selEnd_ = selEnd;
			return edit;
		}

		void Edit::createDrawingResources(Drawing::Context &ctx)
		{
		}

		void Edit::discardDrawingResources(Drawing::Context &ctx)
		{
			brushText.release();
			brushPlaceholder.release();
		}

		void Edit::draw(Drawing::Context &ctx)
		{
			HRESULT hr;
			if(!brushText)
			{
				hr = ctx->CreateSolidColorBrush(colorText_, &brushText);
				if(FAILED(hr))
					throw(UIException("CreateSolidColorBrush failed in Edit::draw."));
			}
			if(!brushPlaceholder)
			{
				hr = ctx->CreateSolidColorBrush(colorPlaceholder_, &brushPlaceholder);
				if(FAILED(hr))
					throw(UIException("CreateSolidColorBrush failed in Edit::draw."));
			}
			if(!brushBackground)
			{
				hr = ctx->CreateSolidColorBrush(colorBackground_, &brushBackground);
				if(FAILED(hr))
					throw(UIException("CreateSolidColorBrush failed in Edit::draw."));
			}

			ctx->FillRectangle(
				rect(),
				brushBackground
				);

			ComPtr<IDWriteRenderingParams> originalRenderParams;
			ctx->GetTextRenderingParams(&originalRenderParams);
			ctx->SetTextRenderingParams(renderParams);
			// TODO: provide edit
			if(text_.empty())
			{
				ctx->DrawTextW(
					placeholder_.c_str(),
					static_cast<unsigned>(placeholder_.size()),
					formatPlaceholder,
					rect(),
					brushPlaceholder,
					D2D1_DRAW_TEXT_OPTIONS_NONE,
					DWRITE_MEASURING_MODE_GDI_CLASSIC
					);
			}
			else
			{
				ctx->DrawTextW(
					text_.c_str(),
					static_cast<unsigned>(text_.size()),
					formatText,
					rect(),
					brushText,
					D2D1_DRAW_TEXT_OPTIONS_NONE,
					DWRITE_MEASURING_MODE_GDI_CLASSIC
					);
			}
			ctx->SetTextRenderingParams(originalRenderParams);
		}

		Objects::SizeF Edit::computeSize()
		{
			// TODO: implement
			return Objects::SizeF(64.0f, 64.0f);
		}

		const std::wstring &Edit::text() const
		{
			return text_;
		}

		void Edit::text(const std::wstring &itext)
		{
			text_ = itext;
			// TODO: discard IME mode, ...
			redraw();
		}

		const std::wstring &Edit::placeholder() const
		{
			return placeholder_;
		}

		void Edit::placeholder(const std::wstring &iplaceholder)
		{
			placeholder_ = iplaceholder;
			if(text_.empty())
				redraw();
		}

		std::pair<size_t, size_t> Edit::selection() const
		{
			return std::make_pair(selStart_, selEnd_);
		}

		void Edit::selection(size_t selPos)
		{
			selStart_ = selEnd_ = selPos;
			// TODO: discard IME mode, ...
			redraw();
		}

		void Edit::selection(size_t iselStart, size_t iselEnd)
		{
			selStart_ = iselStart;
			selEnd_ = iselEnd;
			// TODO: discard IME mode, ...
			redraw();
		}
	}
}
