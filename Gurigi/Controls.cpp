#include "Common.h"

#include "../Batang/Utility.h"

#include "Controls.h"
#include "Thread.h"

namespace Gurigi
{
    Label::Label(const ControlId &id)
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
        std::wstring text,
        Objects::ColorF color,
        uint32_t align
        )
    {
        ControlPtr<Label> lbl(new Label(ControlManager::instance().getNextId()));
        lbl->text_ = std::move(text);
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

    void Label::text(std::wstring text)
    {
        text_ = std::move(text);
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

    Button::Button(const ControlId &id)
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
        std::wstring text
        )
    {
        ControlPtr<Button> btn(new Button(ControlManager::instance().getNextId()));
        btn->text_ = std::move(text);
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

    void Button::text(std::wstring text)
    {
        text_ = std::move(text);
        redraw();
    }

    const std::chrono::milliseconds Scrollbar::PagerTimerInitialInterval(400);
    const std::chrono::milliseconds Scrollbar::PagerTimerInterval(100);

    Scrollbar::Scrollbar(const ControlId &id)
        : Control(id)
        , orientation_(Orientation::Vertical)
        , scrollMin_(0.0)
        , scrollMax_(0.0)
        , pageSize_(0.0)
        , current_(0.0)
        , colorThumb_(Objects::ColorF::Black)
        , colorBackground_(Objects::ColorF::LightGray)
        , dragging_(false)
        , draggingThumbValueDiff_(0.0)
    {
        onMouseMove += Batang::delegate(this, &Scrollbar::onMouseMoveImpl);
        onMouseButtonDown += Batang::delegate(this, &Scrollbar::onMouseButtonDownImpl);
        onMouseButtonUp += Batang::delegate(this, &Scrollbar::onMouseButtonUpImpl);
    }

    Scrollbar::~Scrollbar()
    {
        cancelPagerTimer();
    }

    ControlPtr<Scrollbar> Scrollbar::create(
        Orientation orientation,
        double scrollMin, double scrollMax, double pageSize,
        const Objects::ColorF &colorThumb,
        const Objects::ColorF &colorBackground
        )
    {
        ControlPtr<Scrollbar> scrollbar(new Scrollbar(ControlManager::instance().getNextId()));
        scrollbar->orientation_ = orientation;
        scrollbar->scrollMin_ = scrollMin;
        scrollbar->scrollMax_ = scrollMax;
        scrollbar->pageSize_ = pageSize;
        scrollbar->colorThumb_ = colorThumb;
        scrollbar->colorBackground_ = colorBackground;
        return scrollbar;
    }

    void Scrollbar::createDrawingResources(Drawing::Context &ctx)
    {
    }

    void Scrollbar::discardDrawingResources(Drawing::Context &ctx)
    {
        brushThumb_.release();
        brushBackground_.release();
    }

    void Scrollbar::draw(Drawing::Context &ctx)
    {
        HRESULT hr;
        if(!brushThumb_)
        {
            hr = ctx->CreateSolidColorBrush(colorThumb_, &brushThumb_);
            if(FAILED(hr))
                throw(UIException("CreateSolidColorBrush failed in Edit::draw."));
        }
        if(!brushBackground_)
        {
            hr = ctx->CreateSolidColorBrush(colorBackground_, &brushBackground_);
            if(FAILED(hr))
                throw(UIException("CreateSolidColorBrush failed in Edit::draw."));
        }

        // TODO: hover

        auto rc = rect();

        ctx->FillRectangle(
            rc,
            brushBackground_
            );

        if(scrollMin_ < scrollMax_ && pageSize_ > 0.0)
        {
            // TODO: padding

            if(orientation_ == Orientation::Vertical)
            {
                float height = rc.height();
                float pos = static_cast<float>(height * current_ / (scrollMax_ - scrollMin_));
                float length = static_cast<float>(height * pageSize_ / (scrollMax_ - scrollMin_));
                // TODO: min length

                ctx->FillRoundedRectangle(
                    D2D1::RoundedRect(Objects::RectangleF(rc.left, rc.top + pos, rc.right, rc.top + pos + length), rc.width() / 2, rc.width() / 2),
                    brushThumb_
                    );
            }
            else
            {
                // TODO: rtl

                float width = rc.width();
                float pos = static_cast<float>(width * current_ / (scrollMax_ - scrollMin_));
                float length = static_cast<float>(width * pageSize_ / (scrollMax_ - scrollMin_));
                // TODO: min length

                ctx->FillRoundedRectangle(
                    D2D1::RoundedRect(Objects::RectangleF(rc.left + pos, rc.top, rc.left + pos + length, rc.bottom), rc.height() / 2, rc.height() / 2),
                    brushThumb_
                    );
            }
        }
        else if(scrollMin_ == scrollMax_)
        {
            float minHalf = std::min(rc.width(), rc.height()) / 2;

            ctx->FillRoundedRectangle(
                D2D1::RoundedRect(Objects::RectangleF(rc.left, rc.top, rc.right, rc.bottom), minHalf, minHalf),
                brushThumb_
                );
        }
    }

    Objects::SizeF Scrollbar::computeSize()
    {
        // TODO: implement
        if(orientation_ == Orientation::Vertical)
        {
            return Objects::SizeF(16.0f, 64.0f);
        }
        else
        {
            return Objects::SizeF(64.0f, 16.0f);
        }
    }

    double Scrollbar::posToValue(float pos) const
    {
        auto &rc = rect();
        float posStart = (orientation_ == Orientation::Vertical ? rc.top : rc.left);
        float maxSize = (orientation_ == Orientation::Vertical ? rc.height() : rc.width());

        double value = (pos - posStart) * (scrollMax_ - scrollMin_) / maxSize;
        if(value < scrollMin_)
            value = scrollMin_;
        else if(value > scrollMax_)
            value = scrollMax_;
        return value;
    }

    float Scrollbar::valueToPos(double value) const
    {
        if(scrollMin_ == scrollMax_)
            return 0.0f;
        else if(value < scrollMin_)
            value = scrollMin_;
        else if(value > scrollMax_)
            value = scrollMax_;

        auto &rc = rect();
        float posStart = (orientation_ == Orientation::Vertical ? rc.top : rc.left);
        float maxSize = (orientation_ == Orientation::Vertical ? rc.height() : rc.width());

        return static_cast<float>((value - scrollMin_) * maxSize / (scrollMax_ - scrollMin_) + posStart);
    }

    void Scrollbar::setPagerTimer()
    {
        cancelPagerTimer();
        pagerTimer_ = Batang::Timer::instance().installPeriodicTimer(Batang::ThreadTaskPool::current(),
            PagerTimerInitialInterval,
            PagerTimerInterval,
            std::bind(&Scrollbar::onTimerPager, this));
    }

    void Scrollbar::cancelPagerTimer()
    {
        if(pagerTimer_)
        {
            Batang::Timer::instance().uninstallTimer(pagerTimer_);
            pagerTimer_ = Batang::Timer::TaskId::InvalidValue;
        }
    }

    void Scrollbar::onMouseMoveImpl(const ControlEventArg &arg)
    {
        if(dragging_)
        {
            float pos = (orientation_ == Orientation::Vertical ? arg.shellClientPoint.y : arg.shellClientPoint.x);
            double posValue = posToValue(pos);
            double newCurrent = posValue - draggingThumbValueDiff_;
            current(newCurrent);
        }
    }

    void Scrollbar::onMouseButtonDownImpl(const ControlEventArg &arg)
    {
        if(arg.buttonNum == 1)
        {
            float pos = (orientation_ == Orientation::Vertical ? arg.shellClientPoint.y : arg.shellClientPoint.x);
            double posValue = posToValue(pos);

            if(current_ <= posValue && posValue < current_ + pageSize_) // thumb dragging
            {
                dragging_ = true;
                draggingThumbValueDiff_ = posValue - current_;
            }
            else if(posValue < current_) // page up
            {
                current(current_ - pageSize_);
                clickedPagerToMin_ = true;
                setPagerTimer();
            }
            else if(current_ + pageSize_ <= posValue) // page down
            {
                current(current_ + pageSize_);
                clickedPagerToMin_ = false;
                setPagerTimer();
            }
        }
    }

    void Scrollbar::onMouseButtonUpImpl(const ControlEventArg &arg)
    {
        if(arg.buttonNum == 1)
        {
            dragging_ = false;
            clickedPagerToMin_ = boost::indeterminate;
        }
    }

    void Scrollbar::onTimerPager()
    {
        auto lshell = shell().lock();
        if(lshell)
        {
            auto point = lshell->screenToClient(getCursorPos());
            float pos = (orientation_ == Orientation::Vertical ? point.y : point.x);
            double posValue = posToValue(pos);

            if(clickedPagerToMin_)
            {
                if(posValue < current_)
                {
                    current(current_ - pageSize_);
                }
            }
            else if(!clickedPagerToMin_)
            {
                if(current_ + pageSize_ <= posValue)
                {
                    current(current_ + pageSize_);
                }
            }
            else
            {
                cancelPagerTimer();
            }
        }
        else
        {
            cancelPagerTimer();
        }
    }

    std::pair<double, double> Scrollbar::range() const
    {
        return { scrollMin_, scrollMax_ };
    }

    void Scrollbar::range(double scrollMin, double scrollMax)
    {
        scrollMin_ = scrollMin;
        scrollMax_ = scrollMax;

        if(scrollMin_ > scrollMax_)
            std::swap(scrollMin_, scrollMax_);

        current(current_);
    }

    double Scrollbar::pageSize() const
    {
        return pageSize_;
    }

    void Scrollbar::pageSize(double pageSize)
    {
        pageSize_ = pageSize;

        current(current_);
    }

    double Scrollbar::current() const
    {
        return current_;
    }

    void Scrollbar::current(double current)
    {
        bool toFire = false;
        if(current_ != current)
        {
            toFire = true;
        }

        current_ = current;

        if(current_ < scrollMin_)
            current_ = scrollMin_;
        else if(current_ > scrollMax_ - pageSize_)
            current_ = scrollMax_ - pageSize_;

        redraw();

        if(toFire)
        {
            onScroll(current);
        }
    }
}
