#pragma once

#include "Drawing.h"
#include "Layouts.h"
#include "Resources.h"
#include "Window.h"

namespace Gurigi
{
    class FrameWindow : public Shell
    {
    public:
        static const uint32_t windowStyle;
        static const uint32_t windowStyleEx;

    private:
        ControlPtr<ShellLayout> client_; // FrameWindow handles only one child.
        Resources::ResourcePtr<Resources::Icon> iconLarge_, iconSmall_;
        Objects::ColorF bgColor_;
        Objects::SizeF minClientSize_, maxClientSize_;
        Drawing::Context context_;

        std::wstring className;
        std::wstring initTitle;
        Objects::PointI initPosition;
        Objects::SizeF initClientSize;
        bool inDestroy;
        int32_t capturedButtons;
        std::vector<ControlWeakPtr<>> captureds, prevHovereds;

    protected:
        FrameWindow();
        explicit FrameWindow(const ShellWeakPtr<> &);
        virtual ~FrameWindow();

    public:
        static ShellPtr<FrameWindow> create(
            const ShellWeakPtr<> &,
            const std::wstring &,
            const Resources::ResourcePtr<Resources::Icon> &,
            const Resources::ResourcePtr<Resources::Icon> &,
            const Objects::ColorF & = Objects::ColorF(Objects::ColorF::White),
            const Objects::SizeF & = Objects::SizeF::invalid,
            const Objects::PointI & = Objects::PointI::invalid,
            const Objects::SizeF & = Objects::SizeF::invalid,
            const Objects::SizeF & = Objects::SizeF::invalid
            ); // TODO: menu

    public:
        virtual bool show();
        virtual bool show(int32_t);
        virtual Objects::SizeI adjustWindowSize(const Objects::SizeF &) const;
        virtual Objects::PointF screenToClient(const Objects::PointI &) const;
        virtual Objects::PointI clientToScreen(const Objects::PointF &) const;
        virtual void redraw();

    public:
        virtual const ControlPtr<ShellLayout> &client() const;
        virtual const Resources::ResourcePtr<Resources::Icon> &iconLarge() const;
        virtual void iconLarge(const Resources::ResourcePtr<Resources::Icon> &);
        virtual const Resources::ResourcePtr<Resources::Icon> &iconSmall() const;
        virtual void iconSmall(const Resources::ResourcePtr<Resources::Icon> &);
        virtual const Objects::ColorF &bgColor() const;
        virtual void bgColor(const Objects::ColorF &);
        virtual Objects::SizeF clientSize() const;
        virtual void clientSize(const Objects::SizeF &);
        virtual Objects::SizeI windowSize() const;
        virtual void windowSize(const Objects::SizeI &);
        virtual Objects::PointI position() const;
        virtual void position(const Objects::PointI &);
        virtual const Objects::SizeF &minClientSize() const;
        virtual void minClientSize(const Objects::SizeF &);
        virtual const Objects::SizeF &maxClientSize() const;
        virtual void maxClientSize(const Objects::SizeF &);

    private:
        virtual longptr_t procMessage(HWND, unsigned, uintptr_t, longptr_t);
    };
}
