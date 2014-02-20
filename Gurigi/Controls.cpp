#include "Common.h"

#include "Batang/Utility.h"

#include "Controls.h"

namespace Gurigi
{
    Label::Label(const ControlID &id)
        : Control(id)
        , color_(Objects::ColorF::Black)
    {
        format_ = Drawing::FontFactory::instance().createFont(16.0f); // TODO: font size customization?

        const auto &dwfac = Drawing::D2DFactory::instance().getDWriteFactory();
        ComPtr<IDWriteRenderingParams> renderParamsTemp;
        dwfac->CreateRenderingParams(&renderParamsTemp);
        dwfac->CreateCustomRenderingParams(
            renderParamsTemp->GetGamma(),
            renderParamsTemp->GetEnhancedContrast(),
            renderParamsTemp->GetClearTypeLevel(),
            renderParamsTemp->GetPixelGeometry(),
            DWRITE_RENDERING_MODE_CLEARTYPE_GDI_CLASSIC,
            &renderParams_);
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
        brush_.release();
    }

    void Label::draw(Drawing::Context &ctx)
    {
        if(!brush_)
        {
            HRESULT hr;
            hr = ctx->CreateSolidColorBrush(color_, &brush_);
            if(FAILED(hr))
                throw(UIException("CreateSolidColorBrush failed in Label::draw."));
        }

        ComPtr<IDWriteRenderingParams> oldRenderParams;
        ctx->GetTextRenderingParams(&oldRenderParams);
        ctx->SetTextRenderingParams(renderParams_);
        ctx->DrawTextW(
            text_.c_str(),
            static_cast<unsigned>(text_.size()),
            format_,
            rect(),
            brush_,
            D2D1_DRAW_TEXT_OPTIONS_NONE,
            DWRITE_MEASURING_MODE_GDI_CLASSIC);
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
        brush_.release();
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
            format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            break;

        case CENTER:
            format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            break;

        case RIGHT:
            format_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            break;
        }

        switch(align & VERT_MASK)
        {
        case TOP:
            format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            break;

        case VCENTER:
            format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            break;

        case BOTTOM:
            format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
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
        , clicked_(false)
        , hovered_(false)
    {
        onMouseButtonDown += Batang::delegate(this, &Button::onMouseButtonDownImpl);
        onMouseButtonDoubleClick += Batang::delegate(this, &Button::onMouseButtonDownImpl);
        onMouseButtonUp += Batang::delegate(this, &Button::onMouseButtonUpImpl);

        onMouseOver +=
            [this](const ControlEventArg &arg)
            {
                hovered_ = true;
                redraw();
            };

        onMouseOut +=
            [this](const ControlEventArg &arg)
            {
                hovered_ = false;
                redraw();
            };
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
        ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGray), &brushUp_);
        ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &brushDown_);
        ctx->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Silver), &brushHovered_);
    }

    void Button::discardDrawingResources(Drawing::Context &ctx)
    {
        brushUp_.release();
        brushDown_.release();
        brushHovered_.release();
    }

    void Button::draw(Drawing::Context &ctx)
    {
        Objects::RectangleF rect = this->rect();
        if(clicked_)
            ctx->FillRoundedRectangle(D2D1::RoundedRect(rect, 4.0f, 4.0f), brushDown_);
        else if(hovered_)
            ctx->FillRoundedRectangle(D2D1::RoundedRect(rect, 4.0f, 4.0f), brushHovered_);
        else
            ctx->FillRoundedRectangle(D2D1::RoundedRect(rect, 4.0f, 4.0f), brushUp_);
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
            clicked_ = true;
            redraw();
        }
    }

    void Button::onMouseButtonUpImpl(const ControlEventArg &arg)
    {
        if(arg.buttonNum == 1)
        {
            clicked_ = false;
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

    Edit::Edit(const ControlID &id)
        : Control(id)
        , colorText_(Objects::ColorF::Black)
        , colorPlaceholder_(Objects::ColorF::DarkGray)
        , colorBackground_(Objects::ColorF::Black)
        // , renderer(new (std::nothrow) EditRenderer())
        , focused_(false)
        , dragging_(false)
        , trailing_(false)
    {
        formatPlaceholder_ = Drawing::FontFactory::instance().createFont(
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
            &renderParams_
            );

        onMouseMove += Batang::delegate(this, &Edit::onMouseMoveImpl);
        onMouseButtonDown += Batang::delegate(this, &Edit::onMouseButtonDownImpl);
        onMouseButtonUp += Batang::delegate(this, &Edit::onMouseButtonUpImpl);
        onKeyDown += [this](const ControlEventArg &e)
        {
            if(e.keyCode == VK_LEFT)
            {
                auto sel = selection();
                if(sel.second == 0) return;
                if(e.shiftKey)
                {
                    selection(sel.first, sel.second - 1, false);
                }
                else
                {
                    selection(sel.second - 1, false);
                }
            }
            else if(e.keyCode == VK_RIGHT)
            {
                auto sel = selection();
                if(e.shiftKey)
                {
                    selection(sel.first, sel.second + 1, true);
                }
                else
                {
                    selection(sel.second + 1, true);
                }
            }
        };
        onFocus += Batang::delegate(this, &Edit::onFocusImpl);
        onBlur += Batang::delegate(this, &Edit::onBlurImpl);

        selectionEffect_ = ComPtr<EditDrawingEffect>(new EditDrawingEffect(GetSysColor(COLOR_HIGHLIGHTTEXT)));

        // TODO: temp
        editLayout_.fontEmSize(16.0f);
        editLayout_.size(Gurigi::Objects::SizeF(100.0f, 100.0f));
        editLayout_.defaultReadingDirection(DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);

        std::vector<Gurigi::ComPtr<IDWriteTextFormat>> textFormats;

        {
            textFormats.emplace_back();
            Gurigi::ComPtr<IDWriteTextFormat> &textFormat = textFormats.back();
            dwfac->CreateTextFormat(
                L"Segoe UI",
                nullptr,
                DWRITE_FONT_WEIGHT_REGULAR,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                16.0f,
                L"",
                &textFormat);
        }

        {
            textFormats.emplace_back();
            Gurigi::ComPtr<IDWriteTextFormat> &textFormat = textFormats.back();
            dwfac->CreateTextFormat(
                L"Meiryo",
                nullptr,
                DWRITE_FONT_WEIGHT_REGULAR,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                16.0f,
                L"",
                &textFormat);
        }

        {
            textFormats.emplace_back();
            Gurigi::ComPtr<IDWriteTextFormat> &textFormat = textFormats.back();
            dwfac->CreateTextFormat(
                L"Microsoft JhengHei",
                nullptr,
                DWRITE_FONT_WEIGHT_REGULAR,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                16.0f,
                L"",
                &textFormat);
        }

        {
            textFormats.emplace_back();
            Gurigi::ComPtr<IDWriteTextFormat> &textFormat = textFormats.back();
            dwfac->CreateTextFormat(
                L"Microsoft YaHei",
                nullptr,
                DWRITE_FONT_WEIGHT_REGULAR,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                16.0f,
                L"",
                &textFormat);
        }

        {
            textFormats.emplace_back();
            Gurigi::ComPtr<IDWriteTextFormat> &textFormat = textFormats.back();
            dwfac->CreateTextFormat(
                L"Malgun Gothic",
                nullptr,
                DWRITE_FONT_WEIGHT_REGULAR,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                16.0f,
                L"",
                &textFormat);
        }

        editLayout_.textFormats(std::move(textFormats));
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
        ctx->CreateSolidColorBrush(Objects::ColorF(GetSysColor(COLOR_HIGHLIGHT)), &brushSelection_);
    }

    void Edit::discardDrawingResources(Drawing::Context &ctx)
    {
        brushText_.release();
        brushPlaceholder_.release();
        brushBackground_.release();
        brushSelection_.release();
    }

    void Edit::draw(Drawing::Context &ctx)
    {
        HRESULT hr;
        if(!brushText_)
        {
            hr = ctx->CreateSolidColorBrush(colorText_, &brushText_);
            if(FAILED(hr))
                throw(UIException("CreateSolidColorBrush failed in Edit::draw."));
        }
        if(!brushPlaceholder_)
        {
            hr = ctx->CreateSolidColorBrush(colorPlaceholder_, &brushPlaceholder_);
            if(FAILED(hr))
                throw(UIException("CreateSolidColorBrush failed in Edit::draw."));
        }
        if(!brushBackground_)
        {
            hr = ctx->CreateSolidColorBrush(colorBackground_, &brushBackground_);
            if(FAILED(hr))
                throw(UIException("CreateSolidColorBrush failed in Edit::draw."));
        }

        ComPtr<IDWriteRenderingParams> oldRenderParams;
        ctx->GetTextRenderingParams(&oldRenderParams);
        ctx->SetTextRenderingParams(renderParams_);

        D2D1_MATRIX_3X2_F oldTransform;
        ctx->GetTransform(&oldTransform);
        ctx->SetTransform(clientTransform_);
        Objects::RectangleF newRect(Objects::PointF(0.0f, 0.0f), rect().size());
        ctx->FillRectangle(
            newRect,
            brushBackground_
            );

        // TODO: scroll clip layer or geometry

        // TODO: border

        ctx->SetTransform(clientTransform_ * paddingTransform_ * scrollTransform_);

        for(auto it = std::begin(selectionRects_); it != std::end(selectionRects_); ++ it)
        {
            ctx->FillRectangle(*it, brushSelection_);
        }

        if(text_.empty())
        {
            ctx->DrawTextW(
                placeholder_.c_str(),
                static_cast<unsigned>(placeholder_.size()),
                formatPlaceholder_,
                newRect,
                brushPlaceholder_,
                D2D1_DRAW_TEXT_OPTIONS_NONE,
                DWRITE_MEASURING_MODE_GDI_CLASSIC
                );
        }
        else
        {
            /*renderer->drawLayout(
                ctx,
                EditDrawingEffect(colorText_),
                layout,
                newRect
                );*/
            editLayoutSink_.brush(brushText_);
            editLayoutSink_.draw(ctx, Objects::PointF(1.0f, 1.0f));
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
        clientTransform_ = D2D1::Matrix3x2F::Translation(rect.left, rect.top);
        paddingTransform_ = D2D1::Matrix3x2F::Translation(1.0f, 1.0f); // TODO: padding implementation by design
        scrollTransform_ = D2D1::Matrix3x2F::Identity(); // TODO: scroll

        Objects::SizeF size = rect.size();

        editLayout_.size(Objects::SizeF(size.width - 2.0f, size.height - 2.0f));
        editLayout_.flow(editLayoutSource_, editLayoutSink_);

        updateSelection();
        updateCaret();
    }

    void Edit::textRefresh()
    {
        editLayout_.text(text_);
        editLayout_.analyze();
        if(editLayout_.size().width > 0.0f)
        {
            editLayout_.flow(editLayoutSource_, editLayoutSink_);
        }
        /*
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

        layout->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);*/

        updateSelection();
        updateCaret();
    }

    bool Edit::updateCaret()
    {
        ShellPtr<> lshell = shell().lock();
        if(lshell)
        {
            D2D1::Matrix3x2F transform = clientTransform_ * paddingTransform_ * scrollTransform_;

            Objects::PointF offset;
            ComPtr<IDWriteFontFace> fontFace;
            float fontEmSize;
            if(!editLayoutSink_.getTextPosInfo(selEnd_, trailing_, offset, fontFace, fontEmSize))
            {
                return false;
            }

            DWRITE_FONT_METRICS metrics;
            fontFace->GetMetrics(&metrics);

            uint32_t caretWidth = 2;
            SystemParametersInfoW(SPI_GETCARETWIDTH, 0, &caretWidth, FALSE);

            Objects::RectangleF caretRect(
                Objects::PointF(offset.x, offset.y - (metrics.ascent * fontEmSize / metrics.designUnitsPerEm)),
                Objects::SizeF(static_cast<float>(caretWidth),
                    (metrics.ascent + metrics.descent) * fontEmSize / metrics.designUnitsPerEm));

            D2D1_POINT_2F caretPosF = transform.TransformPoint(caretRect.leftTop());
            Objects::PointI caretPosI = Objects::convertPoint(Objects::PointF(caretPosF.x, caretPosF.y));
            Objects::SizeI caretSize = Objects::convertSize(caretRect.size());

            CreateCaret(lshell->hwnd(), nullptr, caretWidth, caretSize.height);
            SetCaretPos(caretPosI.x, caretPosI.y);

            if(focused_)
                ShowCaret(lshell->hwnd());

            return true;
        }

        return false;
    }

    bool Edit::updateSelection()
    {
        bool ret = false;

        if(selStart_ == selEnd_ && !selectionRects_.empty())
        {
            ret = true;
        }

        selectionRects_.clear();
        if(selStart_ != selEnd_)
        {
            selectionRects_ = editLayoutSink_.getTextSelectionRects(selStart_, selEnd_);

            /*DWRITE_TEXT_RANGE range = { min, max - min };
            layout->SetDrawingEffect(selectionEffect, range);*/

            ret = true;
        }

        return ret;
    }

    void Edit::selectByPoint(const Objects::PointF &pt, bool dragging)
    {
        D2D1::Matrix3x2F inverseTransform = paddingTransform_ * scrollTransform_;
        inverseTransform.Invert();

        D2D1_POINT_2F offset = inverseTransform.TransformPoint(pt);

        bool trailing;
        size_t pos;

        if(!editLayoutSink_.hitTestTextPos(Objects::PointF(offset.x, offset.y), pos, trailing))
            return;

        select(trailing ? SelectMode::AbsoluteTrailing : SelectMode::AbsoluteLeading,
            pos, dragging);
    }

    void Edit::select(SelectMode mode, size_t pos, bool dragging)
    {
    }

    void Edit::onMouseMoveImpl(const ControlEventArg &arg)
    {
        if(!dragging_)
        {
            D2D1::Matrix3x2F inverseTransform = paddingTransform_ * scrollTransform_;
            inverseTransform.Invert();

            D2D1_POINT_2F posTemp = inverseTransform.TransformPoint(arg.controlPoint);
            Objects::PointF pos(posTemp.x, posTemp.y);

            for(auto &rect: selectionRects_)
            {
                if(rect.isIn(pos))
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
            dragging_ = true;
        }
    }

    void Edit::onMouseButtonUpImpl(const ControlEventArg &arg)
    {
        if(arg.buttonNum == 1)
        {
            dragging_ = false;
        }
    }

    void Edit::onFocusImpl(const ControlEventArg &)
    {
        focused_ = true;
        updateCaret();
    }

    void Edit::onBlurImpl(const ControlEventArg &)
    {
        focused_ = false;

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
        textRefresh();
        if(selStart_ != 0 || selEnd_ != 0)
            selection(0, false);
        else
            redraw();
    }

    const std::wstring &Edit::placeholder() const
    {
        return placeholder_;
    }

    void Edit::placeholder(const std::wstring &placeholder)
    {
        placeholder_ = placeholder;
        if(text_.empty())
            redraw();
    }

    std::pair<size_t, size_t> Edit::selection() const
    {
        return std::make_pair(selStart_, selEnd_);
    }

    void Edit::selection(size_t selPos, bool trailing)
    {
        if(selPos > text_.size())
            selPos = text_.size();
        selStart_ = selEnd_ = selPos;
        trailing_ = trailing;
        // TODO: discard IME mode, ...
        if(updateSelection())
        {
            redraw();
        }
        updateCaret();
    }

    void Edit::selection(size_t selStart, size_t selEnd, bool trailing)
    {
        selStart_ = selStart;
        if(selStart_ > text_.size())
            selStart_ = text_.size();
        selEnd_ = selEnd;
        if(selEnd_ > text_.size())
            selEnd_ = text_.size();
        trailing_ = trailing;
        // TODO: discard IME mode, ...
        if(updateSelection())
        {
            redraw();
        }
        updateCaret();
    }
}
