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
        static const uint32_t WindowStyle;
        static const uint32_t WindowStyleEx;

    private:
        ControlPtr<ShellLayout> client_; // FrameWindow handles only one child.
        Resources::ResourcePtr<Resources::Icon> iconLarge_, iconSmall_;
        Objects::ColorF bgColor_;
        Objects::SizeF minClientSize_, maxClientSize_;
        Drawing::Context context_;

        std::wstring className_;
        std::wstring initTitle_;
        Objects::PointI initPosition_;
        Objects::SizeF initClientSize_;
        bool inDestroy_;
        int32_t capturedButtons_;
        std::vector<ControlWeakPtr<>> captureds_, prevHovereds_;

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
            const Objects::SizeF & = Objects::SizeF::Invalid,
            const Objects::PointI & = Objects::PointI::Invalid,
            const Objects::SizeF & = Objects::SizeF::Invalid,
            const Objects::SizeF & = Objects::SizeF::Invalid
            ); // TODO: menu

    public:
        virtual bool show() override;
        virtual bool show(int32_t);
        virtual Objects::SizeI adjustWindowSize(const Objects::SizeF &) const;
        virtual void redraw() override;

    public: // external event handlers
        CommonEvent onTaskProcessable;

    public:
        virtual const ControlPtr<ShellLayout> &client() const;
        virtual const Resources::ResourcePtr<Resources::Icon> &iconLarge() const;
        virtual void iconLarge(const Resources::ResourcePtr<Resources::Icon> &);
        virtual const Resources::ResourcePtr<Resources::Icon> &iconSmall() const;
        virtual void iconSmall(const Resources::ResourcePtr<Resources::Icon> &);
        virtual const Objects::ColorF &bgColor() const;
        virtual void bgColor(const Objects::ColorF &);
        virtual Objects::SizeF clientSize() const override;
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
