#pragma once

#include "Window.h"

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

        ComPtr<ID2D1SolidColorBrush> brush;
        ComPtr<IDWriteTextFormat> format;
        ComPtr<IDWriteRenderingParams> renderParams;

    protected:
        Label(const ControlID &);
        virtual ~Label();

    public:
        static ControlPtr<Label> create(
            const std::wstring &,
            const Objects::ColorF & = Objects::ColorF(Objects::ColorF::Black),
            uint32_t = LEFT | VCENTER
            );

    public:
        virtual void createDrawingResources(Drawing::Context &);
        virtual void discardDrawingResources(Drawing::Context &);
        virtual void draw(Drawing::Context &);
        virtual Objects::SizeF computeSize();

    public:
        virtual const std::wstring &text() const;
        virtual void text(const std::wstring &);
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

        ComPtr<ID2D1SolidColorBrush> brushUp, brushDown, brushHovered;
        bool hovered, clicked;
        Objects::PointF pt;

    protected:
        Button(const ControlID &);
        virtual ~Button();

    public:
        static ControlPtr<Button> create(
            const std::wstring &
            );

    public:
        virtual void createDrawingResources(Drawing::Context &);
        virtual void discardDrawingResources(Drawing::Context &);
        virtual void draw(Drawing::Context &);
        virtual Objects::SizeF computeSize();

    private:
        void onMouseButtonDownImpl(const ControlEventArg &);
        void onMouseButtonUpImpl(const ControlEventArg &);

    public:
        ControlEvent onClick;

    public:
        virtual const std::wstring &text() const;
        virtual void text(const std::wstring &);
    };

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

    class EditLayout
    {
    private:
        std::wstring text_;
        std::vector<ComPtr<IDWriteTextFormat>> textFormats_;
        std::vector<ComPtr<IDWriteFont>> fonts;
        std::vector<ComPtr<IDWriteFontFace>> fontFaces;
        DWRITE_READING_DIRECTION readingDirection_;

    public:
        EditLayout();
        ~EditLayout();

    public:
        void draw(void *, IDWriteTextRenderer *, const Objects::PointF &);

    private:

    public:
        const std::wstring &text() const;
        void text(const std::wstring &);
        const std::vector<ComPtr<IDWriteTextFormat>> &textFormats() const;
        void textFormats(const std::vector<ComPtr<IDWriteTextFormat>> &);
        void textFormats(std::vector<ComPtr<IDWriteTextFormat>> &&);
        DWRITE_READING_DIRECTION readingDirection() const;
        void readingDirection(DWRITE_READING_DIRECTION);

    private:
        void analyze();
    };

    class Edit : public Control
    {
    private:
        enum SelectMode
        {
            SelectModeAbsoluteLeading,
            SelectModeAbsoluteTrailing
        };

    public:
        enum ReadingDirection
        {
            ReadingDirectionSystemDefault, ReadingDirectionLtr, ReadingDirectionRtl
        };

    private:
        std::wstring text_, placeholder_;
        uint32_t selStart_, selEnd_;
        Objects::ColorF colorText_, colorPlaceholder_, colorBackground_;
        ReadingDirection readingDirection_;

        ComPtr<ID2D1SolidColorBrush> brushText, brushPlaceholder, brushBackground, brushSelection;
        ComPtr<IDWriteTextFormat> formatText, formatPlaceholder;
        ComPtr<IDWriteTextLayout> layout;
        ComPtr<EditRenderer> renderer;
        ComPtr<EditDrawingEffect> selectionEffect;
        ComPtr<IDWriteRenderingParams> renderParams;

        std::vector<Objects::RectangleF> selectionRects;

        D2D1::Matrix3x2F clientTransform, paddingTransform, scrollTransform;
        bool focused, dragging, trailing;

    protected:
        Edit(const ControlID &);
        virtual ~Edit();

    public:
        static ControlPtr<Edit> create(
            const std::wstring & = std::wstring(),
            const Objects::ColorF & = Objects::ColorF(Objects::ColorF::Black),
            const Objects::ColorF & = Objects::ColorF(Objects::ColorF::DarkGray),
            const Objects::ColorF & = Objects::ColorF(Objects::ColorF::White),
            const std::wstring & = std::wstring(),
            uint32_t = 0,
            uint32_t = 0
            );

    public:
        virtual void createDrawingResources(Drawing::Context &);
        virtual void discardDrawingResources(Drawing::Context &);
        virtual void draw(Drawing::Context &);
        virtual Objects::SizeF computeSize();

    public:
        virtual void onResizeInternal(const Objects::RectangleF &);

    private:
        void textRefresh();
        bool updateCaret();
        void updateSelection();
        void selectByPoint(const Objects::PointF &, bool);
        void select(SelectMode, uint32_t, bool);

    private:
        void onMouseMoveImpl(const ControlEventArg &);
        void onMouseButtonDownImpl(const ControlEventArg &);
        void onMouseButtonUpImpl(const ControlEventArg &);
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
        virtual void text(const std::wstring &);
        virtual const std::wstring &placeholder() const;
        virtual void placeholder(const std::wstring &);
        virtual std::pair<uint32_t, uint32_t> selection() const;
        virtual void selection(uint32_t, bool);
        virtual void selection(uint32_t, uint32_t, bool);
    };

    class Scrollbar : public Control
    {
    private:
        double scrollMin_, scrollMax_, pageSize_; // [scrollMin, scrollMax - pageSize]

    protected:
        Scrollbar(const ControlID &);
        virtual ~Scrollbar();

    public:
        virtual std::pair<double, double> range() const;
        virtual void range(double, double);
        virtual double pageSize() const;
        virtual void pageSize(double);
    };
}
