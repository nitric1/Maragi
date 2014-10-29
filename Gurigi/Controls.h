#pragma once

#include "Batang/Event.h"
#include "Batang/Timer.h"
#include "Window.h"
#include "Layouts.h"
#include "Detail/EditLayout.h"

namespace Gurigi
{
    class Label : public Control
    {
    public:
        enum
        {
            LEFT = 0x00000000,
            CENTER = 0x00000001,
            RIGHT = 0x00000002,
            TOP = 0x00000000,
            VCENTER = 0x00000004,
            BOTTOM = 0x00000008
        };

        enum
        {
            HORZ_MASK = 0x00000003,
            VERT_MASK = 0x0000000C
        };

    private:
        std::wstring text_;
        Objects::ColorF color_;
        uint32_t align_;

        ComPtr<ID2D1SolidColorBrush> brush_;
        ComPtr<IDWriteTextFormat> format_;
        ComPtr<IDWriteRenderingParams> renderParams_;

    protected:
        Label(const ControlId &);
        virtual ~Label();

    public:
        static ControlPtr<Label> create(
            std::wstring,
            Objects::ColorF = Objects::ColorF(Objects::ColorF::Black),
            uint32_t = LEFT | VCENTER
            );

    public:
        virtual void createDrawingResources(Drawing::Context &) override;
        virtual void discardDrawingResources(Drawing::Context &) override;
        virtual void draw(Drawing::Context &) override;
        virtual Objects::SizeF computeSize() override;

    public:
        virtual const std::wstring &text() const;
        virtual void text(std::wstring);
        virtual const Objects::ColorF &color() const;
        virtual void color(const Objects::ColorF &);
        virtual uint32_t align() const;
        virtual void align(uint32_t);
        virtual bool focusable() const;
    };

    class Button : public Control
    {
    private:
        std::wstring text_;

        ComPtr<ID2D1SolidColorBrush> brushUp_, brushDown_, brushHovered_;
        bool hovered_, clicked_;
        Objects::PointF pt_;

    protected:
        Button(const ControlId &);
        virtual ~Button();

    public:
        static ControlPtr<Button> create(
            std::wstring
            );

    public:
        virtual void createDrawingResources(Drawing::Context &) override;
        virtual void discardDrawingResources(Drawing::Context &) override;
        virtual void draw(Drawing::Context &) override;
        virtual Objects::SizeF computeSize() override;

    private:
        void onMouseButtonDownImpl(const ControlEventArg &);
        void onMouseButtonUpImpl(const ControlEventArg &);

    public:
        ControlEvent onClick;

    public:
        virtual const std::wstring &text() const;
        virtual void text(std::wstring);
    };

    // TODO: Move to Detail or some other namespace
    class DECLSPEC_UUID("809B62F5-ABC3-4FF2-B9F8-7CDC8F78D790") EditDrawingEffect
        : public ComBase<ComBaseListSelf<EditDrawingEffect, ComBaseList<IUnknown>>>
    {
    private:
        Objects::ColorF color_;

    public:
        EditDrawingEffect(const Objects::ColorF &);
        virtual ~EditDrawingEffect();

    public:
        virtual const Objects::ColorF &color() const;
        virtual void color(const Objects::ColorF &);
    };

    class DECLSPEC_UUID("C01E969C-1CCA-41CB-B019-3E0E231C6232") EditRenderer
        : public ComBase<ComBaseListSelf<EditRenderer, ComBaseList<IDWriteTextRenderer>>>
    {
    private:
        ComPtr<ID2D1SolidColorBrush> brush;
        Drawing::Context *renderTarget;
        const EditDrawingEffect *defaultDrawingEffect;

    public:
        EditRenderer();
        virtual ~EditRenderer();

    public:
        HRESULT drawLayout(Drawing::Context &, const EditDrawingEffect &, IDWriteTextLayout *, const Objects::RectangleF &);

    public:
        virtual HRESULT __stdcall DrawGlyphRun(void *, float, float, DWRITE_MEASURING_MODE, const DWRITE_GLYPH_RUN *, const DWRITE_GLYPH_RUN_DESCRIPTION *, IUnknown *);
        virtual HRESULT __stdcall DrawUnderline(void *, float, float, const DWRITE_UNDERLINE *, IUnknown *);
        virtual HRESULT __stdcall DrawStrikethrough(void *, float, float, const DWRITE_STRIKETHROUGH *, IUnknown *);
        virtual HRESULT __stdcall DrawInlineObject(void *, float, float, IDWriteInlineObject *, BOOL, BOOL, IUnknown *);
        virtual HRESULT __stdcall GetCurrentTransform(void *, DWRITE_MATRIX *);
        virtual HRESULT __stdcall GetPixelsPerDip(void *, float *);
        virtual HRESULT __stdcall IsPixelSnappingDisabled(void *, BOOL *);
    };

    class Edit : public EmbeddedLayoutHost<Placer::Grid<1, 2>>
    {
    public:
        enum class ReadingDirection : uint8_t
        {
            SystemDefault, Ltr, Rtl
        };

    private:
        std::wstring text_, placeholder_;
        size_t selStart_, selEnd_;
        Objects::ColorF colorText_, colorPlaceholder_, colorBackground_;
        ReadingDirection readingDirection_;

        Detail::EditLayout editLayout_;
        Detail::EditLayoutSource editLayoutSource_;
        Detail::EditLayoutSink editLayoutSink_;

        ComPtr<ID2D1SolidColorBrush> brushText_, brushPlaceholder_, brushBackground_, brushSelection_;
        ComPtr<IDWriteTextFormat> formatPlaceholder_;
        ComPtr<EditRenderer> renderer_;
        ComPtr<EditDrawingEffect> selectionEffect_;
        ComPtr<IDWriteRenderingParams> renderParams_;
        ComPtr<ID2D1RectangleGeometry> clipGeometry_;
        ComPtr<ID2D1Layer> clipLayer_;

        std::vector<Objects::RectangleF> selectionRects_;

        D2D1::Matrix3x2F clientTransform_, paddingTransform_, scrollTransform_;
        bool focused_, dragging_, trailing_;
        wchar_t firstSurrogatePair_;

        Placer::Grid<1, 2> placer_;
        ControlPtr<Scrollbar> scrollbar_;

    protected:
        Edit(const ControlId &);
        virtual ~Edit();

    public:
        static ControlPtr<Edit> create(
            std::wstring = std::wstring(),
            const Objects::ColorF & = Objects::ColorF(Objects::ColorF::Black),
            const Objects::ColorF & = Objects::ColorF(Objects::ColorF::DarkGray),
            const Objects::ColorF & = Objects::ColorF(Objects::ColorF::White),
            std::wstring = std::wstring(),
            size_t = 0,
            size_t = 0
            );

    public:
        virtual void createDrawingResourcesSelf(Drawing::Context &) override;
        virtual void discardDrawingResourcesSelf(Drawing::Context &) override;
        virtual void drawSelf(Drawing::Context &) override;
        virtual Objects::SizeF computeSize() override;

    public:
        virtual void onResizeInternalSelf(const Objects::RectangleF &);

    private:
        void textRefresh();
        bool updateCaret();
        bool updateSelection();
        void selectByPoint(const Objects::PointF &, bool);
        void select(size_t, bool, bool);

    private:
        void onMouseMoveImpl(const ControlEventArg &);
        void onMouseButtonDownImpl(const ControlEventArg &);
        void onMouseButtonUpImpl(const ControlEventArg &);
        void onMouseWheelImpl(const ControlEventArg &);
        void onKeyDownImpl(const ControlEventArg &);
        void onCharImpl(const ControlEventArg &);
        void onImeEndCompositionImpl(const ControlEventArg &);
        void onImeCompositionImpl(const ControlEventArg &);
        void onFocusImpl(const ControlEventArg &);
        void onBlurImpl(const ControlEventArg &);

    public:
        ControlEvent onTextChange;
        ControlEvent onSelectionChange;

    public:
        virtual const std::wstring &text() const;
        virtual void text(std::wstring);
        virtual const std::wstring &placeholder() const;
        virtual void placeholder(std::wstring);
        virtual std::pair<size_t, size_t> selection() const;
        virtual void selection(size_t, bool);
        virtual void selection(size_t, size_t, bool);
    };

    class Scrollbar : public Control
    {
    public:
        enum class Orientation : uint8_t
        {
            Vertical, Horizontal
        };

    private:
        static const std::chrono::milliseconds PagerTimerInitialInterval;
        static const std::chrono::milliseconds PagerTimerInterval;

    private:
        double scrollMin_, scrollMax_, pageSize_; // [scrollMin, scrollMax - pageSize]
        double current_;
        Orientation orientation_;
        Objects::ColorF colorThumb_, colorBackground_;

        ComPtr<ID2D1SolidColorBrush> brushThumb_, brushBackground_;

        bool dragging_;
        boost::tribool clickedPagerToMin_;
        Batang::Timer::TaskId pagerTimer_;
        double draggingThumbValueDiff_;

    protected:
        Scrollbar(const ControlId &);
        virtual ~Scrollbar();

    public:
        static ControlPtr<Scrollbar> create(
            Orientation,
            double, double, double,
            const Objects::ColorF & = Objects::ColorF(Objects::ColorF::Black),
            const Objects::ColorF & = Objects::ColorF(Objects::ColorF::LightGray)
            );

    public:
        virtual void createDrawingResources(Drawing::Context &) override;
        virtual void discardDrawingResources(Drawing::Context &) override;
        virtual void draw(Drawing::Context &) override;
        virtual Objects::SizeF computeSize() override;

    private:
        double posToValue(float) const;
        float valueToPos(double) const;
        void setPagerTimer();
        void cancelPagerTimer();

    private:
        void onMouseMoveImpl(const ControlEventArg &);
        void onMouseButtonDownImpl(const ControlEventArg &);
        void onMouseButtonUpImpl(const ControlEventArg &);
        void onTimerPager();

    public:
        Batang::Event<double> onScroll;

    public:
        virtual std::pair<double, double> range() const;
        virtual void range(double, double);
        virtual double pageSize() const;
        virtual void pageSize(double);
        virtual double current() const;
        virtual void current(double);
    };
}
