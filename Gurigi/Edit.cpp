#include "Common.h"

#include "Batang/Utility.h"

#include "Controls.h"

// TODO: Scrolling
// TODO: IME
// TODO: Speed tuning

namespace Gurigi
{
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

    Edit::Edit(const ControlId &id)
        : EmbeddedLayoutHost(id, placer_)
        , colorText_(Objects::ColorF::Black)
        , colorPlaceholder_(Objects::ColorF::DarkGray)
        , colorBackground_(Objects::ColorF::White)
        // , renderer(new (std::nothrow) EditRenderer())
        , focused_(false)
        , dragging_(false)
        , trailing_(false)
        , firstSurrogatePair_(L'\0')
        , placer_({ GridSize() }, { GridSize(), GridSize(10.0f) })
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
        onMouseWheel += Batang::delegate(this, &Edit::onMouseWheelImpl);
        onKeyDown += Batang::delegate(this, &Edit::onKeyDownImpl);
        onChar += Batang::delegate(this, &Edit::onCharImpl);
        onFocus += Batang::delegate(this, &Edit::onFocusImpl);
        onBlur += Batang::delegate(this, &Edit::onBlurImpl);

        selectionEffect_ = ComPtr<EditDrawingEffect>(new EditDrawingEffect(GetSysColor(COLOR_HIGHLIGHTTEXT)));

        // TODO: temporary; should be read from config
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
        std::wstring placeholder,
        const Objects::ColorF &colorText,
        const Objects::ColorF &colorPlaceholder,
        const Objects::ColorF &colorBackground,
        std::wstring text,
        size_t selStart,
        size_t selEnd
        )
    {
        ControlPtr<Edit> edit(new Edit(ControlManager::instance().getNextId()));
        edit->placeholder_ = std::move(placeholder);
        edit->colorText_ = colorText;
        edit->colorPlaceholder_ = colorPlaceholder;
        edit->colorBackground_ = colorBackground;
        edit->text_ = std::move(text);
        edit->selStart_ = selStart;
        edit->selEnd_ = selEnd;

        edit->scrollbar_ = Scrollbar::create(Scrollbar::Orientation::Vertical,
            0.0, 1.0, 1.0);
        edit->attach({0, 1}, edit->scrollbar_); // TODO: in rtl orientation, scrollbar should go left
        return edit;
    }

    void Edit::createDrawingResourcesSelf(Drawing::Context &ctx)
    {
        ctx->CreateSolidColorBrush(Objects::ColorF(GetSysColor(COLOR_HIGHLIGHT)), &brushSelection_);
    }

    void Edit::discardDrawingResourcesSelf(Drawing::Context &ctx)
    {
        brushText_.release();
        brushPlaceholder_.release();
        brushBackground_.release();
        brushSelection_.release();
        clipLayer_.release();
    }

    void Edit::drawSelf(Drawing::Context &ctx)
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

        if(!clipLayer_)
        {
            hr = ctx->CreateLayer(nullptr, &clipLayer_);
            if(FAILED(hr))
                throw(UIException("CreateLayer failed in Edit::draw."));
        }

        D2D1_MATRIX_3X2_F oldTransform;
        ctx->GetTransform(&oldTransform);
        ctx->SetTransform(clientTransform_);

        Objects::RectangleF newRect(Objects::PointF(0.0f, 0.0f), rect().size().expand(-10.0f, 0.0f));
        ctx->FillRectangle(
            newRect,
            brushBackground_
            );

        ctx->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clipGeometry_), clipLayer_);

        // TODO: border

        ctx->SetTransform(clientTransform_ * paddingTransform_ * scrollTransform_);

        for(auto it = std::begin(selectionRects_); it != std::end(selectionRects_); ++ it)
        {
            ctx->FillRectangle(*it, brushSelection_);
        }

        if(text_.empty())
        {
            // TODO: use EditLayout
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
            editLayoutSink_.draw(ctx, Objects::PointF(1.0f, 1.0f)); // TODO: if scrollbar went left, point must be added
        }

        ctx->PopLayer();

        ctx->SetTransform(oldTransform);
        ctx->SetTextRenderingParams(oldRenderParams);
    }

    Objects::SizeF Edit::computeSize()
    {
        // TODO: implement
        return Objects::SizeF(64.0f, 20.0f);
    }

    void Edit::onResizeInternalSelf(const Objects::RectangleF &rect)
    {
        clientTransform_ = D2D1::Matrix3x2F::Translation(rect.left, rect.top);
        paddingTransform_ = D2D1::Matrix3x2F::Translation(1.0f, 1.0f); // TODO: padding implementation by design
        scrollTransform_ = D2D1::Matrix3x2F::Identity(); // TODO: scroll

        Objects::SizeF size = rect.size();
        Objects::SizeF clientSize(size.width - 2.0f - scrollbar_->rect().width(), size.height - 2.0f);

        editLayout_.size(clientSize);
        editLayout_.flow(editLayoutSource_, editLayoutSink_);

        Objects::RectangleF clientRect(1.0f, 1.0f, clientSize.width, clientSize.height);

        HRESULT hr;
        hr = Drawing::D2DFactory::instance().getD2DFactory()->CreateRectangleGeometry(clientRect, &clipGeometry_);
        if(FAILED(hr))
            throw(UIException("CreateRectangleGeometry failed in Edit::onResizeInternal."));

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
                DestroyCaret();
                return false;
            }

            uint32_t caretWidth = 2;
            SystemParametersInfoW(SPI_GETCARETWIDTH, 0, &caretWidth, FALSE);

            Objects::RectangleF caretRect(
                Objects::PointF(offset.x - caretWidth / 2.0f, offset.y - editLayoutSink_.ascent()),
                Objects::SizeF(static_cast<float>(caretWidth), editLayoutSink_.textHeight()));

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

        select(pos, trailing, dragging);
    }

    void Edit::select(size_t pos, bool trailing, bool dragging)
    {
        auto sel = selection();
        if(dragging)
        {
            if(sel.second == pos && trailing_ == trailing)
                return;
            selection(selection().first, pos, trailing);
        }
        else
        {
            if(sel.first == pos && sel.second == pos && trailing_ == trailing)
                return;
            selection(pos, trailing);
        }
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

    void Edit::onMouseWheelImpl(const ControlEventArg &arg)
    {
    }

    void Edit::onKeyDownImpl(const ControlEventArg &e)
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
        else if(e.keyCode == VK_DELETE)
        {
            // TODO: split function
            auto sel = selection();
            if(sel.first != sel.second)
            {
                if(sel.first > sel.second)
                {
                    std::swap(sel.first, sel.second);
                }
                text_.erase(text_.begin() + sel.first, text_.begin() + sel.second);
            }
            else if(sel.first < text_.size())
            {
                // TODO: consider surrogate pair
                text_.erase(text_.begin() + sel.first, text_.begin() + sel.first + 1);
            }
            else
            {
                return;
            }

            textRefresh();
            selection(sel.first, false);
            redraw();
            return;
        }
        else if(e.keyCode == VK_BACK)
        {
            // TODO: split function
            auto sel = selection();
            size_t selAfter = 0;
            if(sel.first != sel.second)
            {
                if(sel.first > sel.second)
                {
                    std::swap(sel.first, sel.second);
                }
                text_.erase(text_.begin() + sel.first, text_.begin() + sel.second);
                selAfter = sel.first;
            }
            else if(sel.first > 0)
            {
                // TODO: consider surrogate pair
                text_.erase(text_.begin() + sel.first - 1, text_.begin() + sel.first);
                selAfter = sel.first - 1;
            }
            else
            {
                return;
            }

            textRefresh();
            selection(selAfter, false);
            redraw();
            return;
        }
    }

    void Edit::onCharImpl(const ControlEventArg &e)
    {
        // TODO: split function

        e.stopPropagation();

        char32_t charCode = e.charCode;

        if(charCode == 1) // Ctrl+A
        {
            selection(0, text_.size(), true);
            redraw();
            return;
        }
        else if(charCode < 32) // cannot insert
        {
            return;
        }

        if(0xD800 <= charCode && charCode <= 0xDFFF) // surrogate pair
        {
            if(firstSurrogatePair_)
            {
                if(charCode <= 0xDBFF) // high-surrogate, which must not be here
                {
                    return;
                }

                charCode = (static_cast<char32_t>(firstSurrogatePair_ & 0x03FF) << 10);
                charCode += 0x00010000;
                charCode |= (e.charCode & 0x03FF);

                firstSurrogatePair_ = L'\0';
            }
            else
            {
                if(0xDC00 <= charCode) // low-surrogate, which must not be here
                {
                    return;
                }

                firstSurrogatePair_ = static_cast<wchar_t>(charCode);
                return;
            }
        }

        auto sel = selection();
        if(sel.first > sel.second)
        {
            std::swap(sel.first, sel.second);
        }

        if(sel.first < sel.second)
        {
            text_.erase(text_.begin() + sel.first, text_.begin() + sel.second);
        }

        std::wstring toInsert = Batang::decodeUtf32(std::u32string(1, charCode));
        text_.insert(text_.begin() + sel.first, toInsert.begin(), toInsert.end());

        textRefresh();
        selection(sel.first + toInsert.size(), true);
        redraw();
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

    void Edit::text(std::wstring text)
    {
        text_ = std::move(text);
        // TODO: discard IME mode, ...
        textRefresh();
        if(selStart_ != 0 || selEnd_ != 0)
            selection(0, false);
        redraw();
    }

    const std::wstring &Edit::placeholder() const
    {
        return placeholder_;
    }

    void Edit::placeholder(std::wstring placeholder)
    {
        placeholder_ = std::move(placeholder);
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
