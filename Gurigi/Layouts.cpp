#include "Common.h"

#include "Layouts.h"

namespace Gurigi
{
    Layout::Layout(const ControlId &id)
        : Control(id)
    {
    }

    void Layout::setParent(const ControlPtr<> &control)
    {
        setParent(control, sharedFromThis());
    }

    void Layout::setParent(const ControlPtr<> &control, const ControlWeakPtr<> &parent)
    {
        if(parent.expired())
            return;
        control->shell(parent.lock()->shell());
        control->parent(parent);
    }

    void Layout::clearParent(const ControlPtr<> &control)
    {
        control->shell(nullptr);
        control->parent(nullptr);
    }

    void Layout::setShell(const ControlPtr<> &control, const ShellWeakPtr<> &shell)
    {
        control->shell(shell);
    }

    ShellLayout::ShellLayout(const ShellWeakPtr<> &ishell, const ControlId &id)
        : Layout(id)
    {
        shell(ishell);
    }

    ShellLayout::~ShellLayout()
    {
    }

    ControlPtr<ShellLayout> ShellLayout::create(
        const ShellWeakPtr<> &shell
        )
    {
        ControlPtr<ShellLayout> layout(new ShellLayout(shell, ControlManager::instance().getNextId()));
        return layout;
    }

    void ShellLayout::createDrawingResources(Drawing::Context &ctx)
    {
        if(child_)
            child_->createDrawingResources(ctx);
    }

    void ShellLayout::discardDrawingResources(Drawing::Context &ctx)
    {
        if(child_)
            child_->discardDrawingResources(ctx);
    }

    void ShellLayout::draw(Drawing::Context &ctx)
    {
        if(child_)
            child_->draw(ctx);
    }

    Objects::SizeF ShellLayout::computeSize()
    {
        if(child_)
            return child_->computeSize();
        return Objects::SizeF();
    }

    ControlWeakPtr<> ShellLayout::findByPoint(const Objects::PointF &pt)
    {
        if(child_)
            return child_->findByPoint(pt);
        return nullptr;
    }

    std::vector<ControlWeakPtr<>> ShellLayout::findTreeByPoint(const Objects::PointF &pt)
    {
        if(!rect().isIn(pt))
            return std::vector<ControlWeakPtr<>>();

        if(child_)
        {
            std::vector<ControlWeakPtr<>> ve = child_->findTreeByPoint(pt);
            ve.insert(ve.begin(), sharedFromThis());
            return ve;
        }
        return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
    }

    std::vector<ControlWeakPtr<>> ShellLayout::findReverseTreeByPoint(const Objects::PointF &pt)
    {
        if(!rect().isIn(pt))
            return std::vector<ControlWeakPtr<>>();

        if(child_)
        {
            std::vector<ControlWeakPtr<>> ve = child_->findReverseTreeByPoint(pt);
            ve.push_back(sharedFromThis());
            return ve;
        }
        return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
    }

    void ShellLayout::walk(const std::function<void (const ControlWeakPtr<> &)> &fn)
    {
        fn(sharedFromThis());
        if(child_)
            child_->walk(fn);
    }

    void ShellLayout::walkReverse(const std::function<void (const ControlWeakPtr<> &)> &fn)
    {
        if(child_)
            child_->walkReverse(fn);
        fn(sharedFromThis());
    }

    void ShellLayout::onResizeInternal(const Objects::RectangleF &rect)
    {
        child_->rect(rect);
    }

    void ShellLayout::shell(const ShellWeakPtr<> &shell)
    {
        Layout::shell(shell);
        if(child_)
            setShell(child_, shell);
    }

    void ShellLayout::attach(const ControlPtr<> &control)
    {
        if(child_)
            detach();
        child_ = control;
        setParent(control);
    }

    ControlPtr<> ShellLayout::detach()
    {
        auto control = child_;
        child_ = nullptr;
        clearParent(control);
        return control;
    }

    PaddingLayout::PaddingLayout(const ControlId &id)
        : Layout(id)
    {}

    PaddingLayout::~PaddingLayout()
    {
    }

    ControlPtr<PaddingLayout> PaddingLayout::create(const Objects::BoundaryF &boundary)
    {
        ControlPtr<PaddingLayout> layout(new PaddingLayout(ControlManager::instance().getNextId()));
        layout->padding(boundary);
        return layout;
    }

    void PaddingLayout::createDrawingResources(Drawing::Context &ctx)
    {
        if(child_)
            child_->createDrawingResources(ctx);
    }

    void PaddingLayout::discardDrawingResources(Drawing::Context &ctx)
    {
        if(child_)
            child_->discardDrawingResources(ctx);
    }

    void PaddingLayout::draw(Drawing::Context &ctx)
    {
        if(child_)
            child_->draw(ctx);
    }

    Objects::SizeF PaddingLayout::computeSize()
    {
        if(child_)
            return child_->computeSize();
        return Objects::SizeF();
    }

    ControlWeakPtr<> PaddingLayout::findByPoint(const Objects::PointF &pt)
    {
        if(child_)
            return child_->findByPoint(pt);
        return nullptr;
    }

    std::vector<ControlWeakPtr<>> PaddingLayout::findTreeByPoint(const Objects::PointF &pt)
    {
        if(!rect().isIn(pt))
            return std::vector<ControlWeakPtr<>>();

        if(child_)
        {
            std::vector<ControlWeakPtr<>> ve = child_->findTreeByPoint(pt);
            ve.insert(ve.begin(), sharedFromThis());
            return ve;
        }
        return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
    }

    std::vector<ControlWeakPtr<>> PaddingLayout::findReverseTreeByPoint(const Objects::PointF &pt)
    {
        if(!rect().isIn(pt))
            return std::vector<ControlWeakPtr<>>();

        if(child_)
        {
            std::vector<ControlWeakPtr<>> ve = child_->findReverseTreeByPoint(pt);
            ve.push_back(sharedFromThis());
            return ve;
        }
        return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
    }

    void PaddingLayout::walk(const std::function<void (const ControlWeakPtr<> &)> &fn)
    {
        fn(sharedFromThis());
        if(child_)
            child_->walk(fn);
    }

    void PaddingLayout::walkReverse(const std::function<void (const ControlWeakPtr<> &)> &fn)
    {
        if(child_)
            child_->walkReverse(fn);
        fn(sharedFromThis());
    }

    void PaddingLayout::onResizeInternal(const Objects::RectangleF &rect)
    {
        Objects::RectangleF childRect(
            rect.left + boundary_.left, rect.top + boundary_.top,
            rect.right - boundary_.right, rect.bottom - boundary_.bottom);
        child_->rect(childRect);
    }

    void PaddingLayout::shell(const ShellWeakPtr<> &shell)
    {
        Layout::shell(shell);
        if(child_)
            setShell(child_, shell);
    }

    Objects::BoundaryF PaddingLayout::padding() const
    {
        return boundary_;
    }

    void PaddingLayout::padding(const Objects::BoundaryF &boundary)
    {
        boundary_ = boundary;
        redraw();
    }

    void PaddingLayout::attach(const ControlPtr<> &control)
    {
        if(child_)
            detach();
        child_ = control;
        setParent(control);
    }

    ControlPtr<> PaddingLayout::detach()
    {
        auto control = child_;
        child_ = nullptr;
        clearParent(control);
        return control;
    }
}
