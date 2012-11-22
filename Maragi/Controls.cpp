// $Id: Controls.cpp 84 2012-11-21 12:06:31Z wdlee91 $

#include "Common.h"

#include "Controls.h"
#include "Utility.h"

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

			ComPtr<IDWriteRenderingParams> oldRenderParams;
			ctx->GetTextRenderingParams(&oldRenderParams);
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
			ctx->SetTextRenderingParams(oldRenderParams);
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

		bool Label::focusable() const
		{
			return false;
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

		EditDrawingEffect::EditDrawingEffect(const Objects::ColorF &color)
			: color_(color)
		{}

		EditDrawingEffect::~EditDrawingEffect()
		{}

		const Objects::ColorF &EditDrawingEffect::color() const
		{
			return color_;
		}

		void EditDrawingEffect::color(const Objects::ColorF &icolor)
		{
			color_ = icolor;
		}

		EditRenderer::EditRenderer()
			: renderTarget(nullptr)
			, defaultDrawingEffect(nullptr)
		{}

		EditRenderer::~EditRenderer()
		{}

		HRESULT EditRenderer::drawLayout(
			Drawing::Context &renderTarget,
			const EditDrawingEffect &defaultDrawingEffect,
			IDWriteTextLayout *layout,
			const Objects::RectangleF &rect
			)
		{
			HRESULT hr;

			brush.release();
			hr = renderTarget->CreateSolidColorBrush(Objects::ColorF(Objects::ColorF::Black), &brush);
			if(FAILED(hr))
				return hr;

			this->renderTarget = &renderTarget;
			this->defaultDrawingEffect = &defaultDrawingEffect;

			hr = layout->Draw(
				nullptr,
				this,
				rect.left,
				rect.top
				);

			this->renderTarget = nullptr;
			this->defaultDrawingEffect = nullptr;
			return hr;
		}

		HRESULT __stdcall EditRenderer::DrawGlyphRun(
			void *,
			float baselineOriginX,
			float baselineOriginY,
			DWRITE_MEASURING_MODE mm,
			const DWRITE_GLYPH_RUN *glyphRun,
			const DWRITE_GLYPH_RUN_DESCRIPTION *glyphRunDescription,
			IUnknown *effect
			)
		{
			const EditDrawingEffect *ede = dynamic_cast<EditDrawingEffect *>(effect);
			if(!ede)
				ede = defaultDrawingEffect;
			brush->SetColor(ede->color());

			(*renderTarget)->DrawGlyphRun(
				Objects::PointF(baselineOriginX, baselineOriginY),
				glyphRun,
				brush,
				mm
				);

			return S_OK;
		}

		HRESULT __stdcall EditRenderer::DrawUnderline(
			void *,
			float baselineOriginX,
			float baselineOriginY,
			const DWRITE_UNDERLINE *underline,
			IUnknown *effect
			)
		{
			const EditDrawingEffect *ede = dynamic_cast<EditDrawingEffect *>(effect);
			if(!ede)
				ede = defaultDrawingEffect;
			brush->SetColor(ede->color());

			Objects::RectangleF rect(
				Objects::PointF(baselineOriginX, baselineOriginY + underline->offset),
				Objects::SizeF(underline->width, underline->thickness)
				);

			(*renderTarget)->FillRectangle(rect, brush);

			return S_OK;
		}

		HRESULT __stdcall EditRenderer::DrawStrikethrough(
			void *,
			float baselineOriginX,
			float baselineOriginY,
			const DWRITE_STRIKETHROUGH *strike,
			IUnknown *effect
			)
		{
			const EditDrawingEffect *ede = dynamic_cast<EditDrawingEffect *>(effect);
			if(!ede)
				ede = defaultDrawingEffect;
			brush->SetColor(ede->color());

			Objects::RectangleF rect(
				Objects::PointF(baselineOriginX, baselineOriginY + strike->offset),
				Objects::SizeF(strike->width, strike->thickness)
				);

			(*renderTarget)->FillRectangle(rect, brush);

			return S_OK;
		}

		HRESULT __stdcall EditRenderer::DrawInlineObject(
			void *,
			float baselineOriginX,
			float baselineOriginY,
			IDWriteInlineObject *object,
			BOOL isSideways,
			BOOL isRtl,
			IUnknown *effect
			)
		{
			const EditDrawingEffect *ede = dynamic_cast<EditDrawingEffect *>(effect);
			if(!ede)
				ede = defaultDrawingEffect;
			brush->SetColor(ede->color());

			// TODO: inline object (image url, ...)

			return S_OK;
		}

		HRESULT __stdcall EditRenderer::GetCurrentTransform(
			void *,
			DWRITE_MATRIX *matrix
			)
		{
			(*renderTarget)->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F *>(matrix));
			return S_OK;
		}

		HRESULT __stdcall EditRenderer::GetPixelsPerDip(
			void *,
			float *pixelsPerDip
			)
		{
			// XXX: consider?
			*pixelsPerDip = 1.0f;
			return S_OK;
		}

		HRESULT __stdcall EditRenderer::IsPixelSnappingDisabled(
			void *,
			BOOL *isDisabled
			)
		{
			*isDisabled = FALSE;
			return S_OK;
		}

		EditLayout::EditLayout()
		{}

		EditLayout::~EditLayout()
		{}

		void EditLayout::draw(void *clientDrawingContext, IDWriteTextRenderer *renderer, const Objects::PointF &origin)
		{
		}

		const std::wstring &EditLayout::text() const
		{
			return text_;
		}

		void EditLayout::text(const std::wstring &itext)
		{
			text_ = itext;
			// TODO: reanalyze
		}

		const std::vector<ComPtr<IDWriteTextFormat>> &EditLayout::textFormats() const
		{
			return textFormats_;
		}

		void EditLayout::textFormats(const std::vector<ComPtr<IDWriteTextFormat>> &itextFormats)
		{
			textFormats_ = itextFormats;
			// TODO: reanalyze
		}

		void EditLayout::textFormats(std::vector<ComPtr<IDWriteTextFormat>> &&itextFormats)
		{
			textFormats_ = std::move(itextFormats);
			// TODO: reanalyze
		}

		Edit::Edit(const ControlID &id)
			: Control(id)
			, colorText_(Objects::ColorF::Black)
			, colorPlaceholder_(Objects::ColorF::DarkGray)
			, colorBackground_(Objects::ColorF::Black)
			, renderer(new (std::nothrow) EditRenderer())
			, focused(false)
			, dragging(false)
			, trailing(false)
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

			onMouseMove += delegate(this, &Edit::onMouseMoveImpl);
			onMouseButtonDown += delegate(this, &Edit::onMouseButtonDownImpl);
			onMouseButtonUp += delegate(this, &Edit::onMouseButtonUpImpl);
			onFocus += delegate(this, &Edit::onFocusImpl);
			onBlur += delegate(this, &Edit::onBlurImpl);

			selectionEffect = new EditDrawingEffect(GetSysColor(COLOR_HIGHLIGHTTEXT));
		}

		Edit::~Edit()
		{}

		ControlPtr<Edit> Edit::create(
			const std::wstring &placeholder,
			const Objects::ColorF &colorText,
			const Objects::ColorF &colorPlaceholder,
			const Objects::ColorF &colorBackground,
			const std::wstring &text,
			uint32_t selStart,
			uint32_t selEnd
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
			ctx->CreateSolidColorBrush(Objects::ColorF(GetSysColor(COLOR_HIGHLIGHT)), &brushSelection);
		}

		void Edit::discardDrawingResources(Drawing::Context &ctx)
		{
			brushText.release();
			brushPlaceholder.release();
			brushBackground.release();
			brushSelection.release();
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

			ComPtr<IDWriteRenderingParams> oldRenderParams;
			ctx->GetTextRenderingParams(&oldRenderParams);
			ctx->SetTextRenderingParams(renderParams);

			D2D1_MATRIX_3X2_F oldTransform;
			ctx->GetTransform(&oldTransform);
			ctx->SetTransform(clientTransform);
			Objects::RectangleF newRect(Objects::PointF(0.0f, 0.0f), rect().size());
			ctx->FillRectangle(
				newRect,
				brushBackground
				);

			// TODO: scroll clip layer or geometry

			// TODO: border

			ctx->SetTransform(clientTransform * paddingTransform * scrollTransform);

			for(auto it = std::begin(selectionRects); it != std::end(selectionRects); ++ it)
				ctx->FillRectangle(*it, brushSelection);

			if(text_.empty())
			{
				ctx->DrawTextW(
					placeholder_.c_str(),
					static_cast<unsigned>(placeholder_.size()),
					formatPlaceholder,
					newRect,
					brushPlaceholder,
					D2D1_DRAW_TEXT_OPTIONS_NONE,
					DWRITE_MEASURING_MODE_GDI_CLASSIC
					);
			}
			else
			{
				renderer->drawLayout(
					ctx,
					EditDrawingEffect(colorText_),
					layout,
					newRect
					);
			}

			ctx->SetTransform(oldTransform);
			ctx->SetTextRenderingParams(oldRenderParams);
		}

		Objects::SizeF Edit::computeSize()
		{
			// TODO: implement
			return Objects::SizeF(64.0f, 64.0f);
		}

		void Edit::onResizeInternal(const Objects::RectangleF &rect)
		{
			clientTransform = D2D1::Matrix3x2F::Translation(rect.left, rect.top);
			paddingTransform = D2D1::Matrix3x2F::Translation(1.0f, 1.0f); // TODO: padding implementation by design
			scrollTransform = D2D1::Matrix3x2F::Identity(); // TODO: scroll

			layout->SetMaxWidth(rect.width());
			layout->SetMaxHeight(rect.height());
			updateSelection();
			updateCaret();
		}

		void Edit::textRefresh()
		{
			layout.release();

			Objects::SizeF size = rect().size();
			const auto &dwfac = Drawing::D2DFactory::instance().getDWriteFactory();
			HRESULT hr = dwfac->CreateGdiCompatibleTextLayout(
				text_.c_str(),
				static_cast<uint32_t>(text_.size()),
				formatText,
				size.width - 2.0f, // TODO: padding
				size.height - 2.0f,
				1.0f, // XXX: implement?
				nullptr,
				FALSE,
				&layout);
			if(FAILED(hr))
				throw(UIException("CreateTextLayout failed in Edit::textRefresh"));

			layout->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);

			updateSelection();
			updateCaret();
		}

		bool Edit::updateCaret()
		{
			ShellPtr<> lshell = shell().lock();
			if(layout && lshell)
			{
				D2D1::Matrix3x2F transform = clientTransform * paddingTransform * scrollTransform;

				DWRITE_HIT_TEST_METRICS htm;
				float x, y;
				layout->HitTestTextPosition(
					selEnd_,
					!!trailing,
					&x,
					&y,
					&htm
					);

				uint32_t caretWidth = 2;
				SystemParametersInfoW(SPI_GETCARETWIDTH, 0, &caretWidth, FALSE);

				Objects::RectangleF caretRect(
					Objects::PointF(x - static_cast<float>(caretWidth) / 2.0f, static_cast<float>(y)),
					Objects::SizeF(static_cast<float>(caretWidth), htm.height)
					);

				D2D1_POINT_2F caretPos = transform.TransformPoint(caretRect.leftTop());

				CreateCaret(lshell->hwnd(), nullptr, static_cast<int>(round(caretRect.width())), static_cast<int>(round(caretRect.height())));
				SetCaretPos(static_cast<int>(round(caretPos.x)), static_cast<int>(round(caretPos.y)));

				if(focused)
					ShowCaret(lshell->hwnd());

				return true;
			}

			return false;
		}

		void Edit::updateSelection()
		{
			selectionRects.clear();
			if(selStart_ != selEnd_)
			{
				uint32_t min = std::min(selStart_, selEnd_), max = std::max(selStart_, selEnd_);
				uint32_t hitTestCount;

				layout->HitTestTextRange(
					min,
					max - min,
					0.0f,
					0.0f,
					nullptr,
					0,
					&hitTestCount
					);

				if(hitTestCount > 0)
				{
					std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(hitTestCount);
					layout->HitTestTextRange(
						min,
						max - min,
						0.0f,
						0.0f,
						hitTestMetrics.data(),
						static_cast<uint32_t>(hitTestMetrics.size()),
						&hitTestCount
						);

					if(hitTestCount > 0)
					{
						for(size_t i = 0; i < hitTestCount; ++ i)
						{
							const auto &htm = hitTestMetrics[i];
							selectionRects.push_back(Objects::RectangleF(
								Objects::PointF(htm.left, htm.top),
								Objects::SizeF(htm.width, htm.height)
								));
						}
					}
				}

				DWRITE_TEXT_RANGE range = { min, max - min };
				layout->SetDrawingEffect(selectionEffect, range);
			}
		}

		void Edit::selectByPoint(const Objects::PointF &pt, bool dragging)
		{
			if(layout)
			{
				D2D1::Matrix3x2F inverseTransform = paddingTransform * scrollTransform;
				inverseTransform.Invert();

				D2D1_POINT_2F pos = inverseTransform.TransformPoint(pt);

				BOOL trailing, inside;
				DWRITE_HIT_TEST_METRICS htm;

				layout->HitTestPoint(
					pos.x,
					pos.y,
					&trailing,
					&inside,
					&htm
					);

				select(
					trailing ? SelectModeAbsoluteTrailing : SelectModeAbsoluteLeading,
					htm.textPosition,
					dragging
					);
			}
		}

		void Edit::select(SelectMode mode, uint32_t pos, bool dragging)
		{
			;
		}

		void Edit::onMouseMoveImpl(const ControlEventArg &arg)
		{
			if(!dragging)
			{
				D2D1::Matrix3x2F inverseTransform = paddingTransform * scrollTransform;
				inverseTransform.Invert();

				D2D1_POINT_2F posTemp = inverseTransform.TransformPoint(arg.controlPoint);
				Objects::PointF pos(posTemp.x, posTemp.y);

				for(auto it = std::begin(selectionRects); it != std::end(selectionRects); ++ it)
				{
					if(it->isIn(pos))
					{
						cursor(Resources::Cursor::Predefined::arrow());
						return;
					}
				}
			}
			else
				selectByPoint(arg.controlPoint, true);

			cursor(Resources::Cursor::Predefined::ibeam());
		}

		void Edit::onMouseButtonDownImpl(const ControlEventArg &arg)
		{
			if(arg.buttonNum == 1)
			{
				selectByPoint(arg.controlPoint, false);
				dragging = true;
			}
		}

		void Edit::onMouseButtonUpImpl(const ControlEventArg &arg)
		{
			if(arg.buttonNum == 1)
			{
				dragging = false;
			}
		}

		void Edit::onFocusImpl(const ControlEventArg &)
		{
			if(updateCaret())
			{
				ShellPtr<> lshell = shell().lock();
				if(lshell)
					ShowCaret(lshell->hwnd());
			}

			focused = true;
		}

		void Edit::onBlurImpl(const ControlEventArg &)
		{
			focused = false;

			DestroyCaret();
		}

		const std::wstring &Edit::text() const
		{
			return text_;
		}

		void Edit::text(const std::wstring &itext)
		{
			text_ = itext;
			// TODO: discard IME mode, ...
			selection(0, false);
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

		std::pair<uint32_t, uint32_t> Edit::selection() const
		{
			return std::make_pair(selStart_, selEnd_);
		}

		void Edit::selection(uint32_t selPos, bool itrailing)
		{
			selStart_ = selEnd_ = selPos;
			trailing = itrailing;
			// TODO: discard IME mode, ...
			textRefresh();
			redraw();
		}

		void Edit::selection(uint32_t iselStart, uint32_t iselEnd, bool itrailing)
		{
			selStart_ = iselStart;
			selEnd_ = iselEnd;
			trailing = itrailing;
			// TODO: discard IME mode, ...
			textRefresh();
			redraw();
		}
	}
}
