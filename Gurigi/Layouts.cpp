#include "Common.h"

#include "Layouts.h"

namespace Gurigi
{
    Layout::Layout(const ControlId &id)
        : Control(id)
    {
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
        ControlPtr<ShellLayout> layout(new ShellLayout(shell, ControlManager::instance().getNextID()));
        layout->slot_.parent(layout);
        return layout;
    }

    void ShellLayout::createDrawingResources(Drawing::Context &ctx)
    {
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            lchild->createDrawingResources(ctx);
    }

    void ShellLayout::discardDrawingResources(Drawing::Context &ctx)
    {
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            lchild->discardDrawingResources(ctx);
    }

    void ShellLayout::draw(Drawing::Context &ctx)
    {
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            lchild->draw(ctx);
    }

    Objects::SizeF ShellLayout::computeSize()
    {
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            return lchild->computeSize();
        return Objects::SizeF();
    }

    ControlWeakPtr<> ShellLayout::findByPoint(const Objects::PointF &pt)
    {
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            return lchild->findByPoint(pt);
        return nullptr;
    }

    std::vector<ControlWeakPtr<>> ShellLayout::findTreeByPoint(const Objects::PointF &pt)
    {
        if(!rect().isIn(pt))
            return std::vector<ControlWeakPtr<>>();

        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
        {
            std::vector<ControlWeakPtr<>> ve = lchild->findTreeByPoint(pt);
            ve.insert(ve.begin(), sharedFromThis());
            return ve;
        }
        return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
    }

    std::vector<ControlWeakPtr<>> ShellLayout::findReverseTreeByPoint(const Objects::PointF &pt)
    {
        if(!rect().isIn(pt))
            return std::vector<ControlWeakPtr<>>();

        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
        {
            std::vector<ControlWeakPtr<>> ve = lchild->findReverseTreeByPoint(pt);
            ve.push_back(sharedFromThis());
            return ve;
        }
        return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
    }

    void ShellLayout::walk(const std::function<void (const ControlWeakPtr<> &)> &fn)
    {
        fn(sharedFromThis());
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            lchild->walk(fn);
    }

    void ShellLayout::walkReverse(const std::function<void (const ControlWeakPtr<> &)> &fn)
    {
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            lchild->walkReverse(fn);
        fn(sharedFromThis());
    }

    void ShellLayout::onResizeInternal(const Objects::RectangleF &rect)
    {
        ControlPtr<> lchild = slot_.child().lock();
        lchild->rect(rect);
    }

    Slot *ShellLayout::slot()
    {
        return &slot_;
    }

    PaddingLayout::PaddingLayout(const ControlId &id)
        : Layout(id)
    {}

    PaddingLayout::~PaddingLayout()
    {
    }

    ControlPtr<PaddingLayout> PaddingLayout::create(float padAll)
    {
        return create(padAll, padAll, padAll, padAll);
    }

    ControlPtr<PaddingLayout> PaddingLayout::create(float padHorz, float padVert)
    {
        return create(padHorz, padVert, padHorz, padVert);
    }

    ControlPtr<PaddingLayout> PaddingLayout::create(float padLeft, float padTop, float padRight, float padBottom)
    {
        ControlPtr<PaddingLayout> layout(new PaddingLayout(ControlManager::instance().getNextID()));
        layout->padding(padLeft, padTop, padRight, padBottom);
        return layout;
    }

    void PaddingLayout::createDrawingResources(Drawing::Context &ctx)
    {
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            lchild->createDrawingResources(ctx);
    }

    void PaddingLayout::discardDrawingResources(Drawing::Context &ctx)
    {
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            lchild->discardDrawingResources(ctx);
    }

    void PaddingLayout::draw(Drawing::Context &ctx)
    {
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            lchild->draw(ctx);
    }

    Objects::SizeF PaddingLayout::computeSize()
    {
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            return lchild->computeSize();
        return Objects::SizeF();
    }

    ControlWeakPtr<> PaddingLayout::findByPoint(const Objects::PointF &pt)
    {
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            return lchild->findByPoint(pt);
        return nullptr;
    }

    std::vector<ControlWeakPtr<>> PaddingLayout::findTreeByPoint(const Objects::PointF &pt)
    {
        if(!rect().isIn(pt))
            return std::vector<ControlWeakPtr<>>();

        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
        {
            std::vector<ControlWeakPtr<>> ve = lchild->findTreeByPoint(pt);
            ve.insert(ve.begin(), sharedFromThis());
            return ve;
        }
        return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
    }

    std::vector<ControlWeakPtr<>> PaddingLayout::findReverseTreeByPoint(const Objects::PointF &pt)
    {
        if(!rect().isIn(pt))
            return std::vector<ControlWeakPtr<>>();

        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
        {
            std::vector<ControlWeakPtr<>> ve = lchild->findReverseTreeByPoint(pt);
            ve.push_back(sharedFromThis());
            return ve;
        }
        return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
    }

    void PaddingLayout::walk(const std::function<void (const ControlWeakPtr<> &)> &fn)
    {
        fn(sharedFromThis());
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            lchild->walk(fn);
    }

    void PaddingLayout::walkReverse(const std::function<void (const ControlWeakPtr<> &)> &fn)
    {
        ControlPtr<> lchild = slot_.child().lock();
        if(lchild)
            lchild->walkReverse(fn);
        fn(sharedFromThis());
    }

    void PaddingLayout::onResizeInternal(const Objects::RectangleF &rect)
    {
        Objects::RectangleF childRect(rect.left + padLeft_, rect.top + padTop_, rect.right - padRight_, rect.bottom - padBottom_);
        ControlPtr<> lchild = slot_.child().lock();
        lchild->rect(childRect);
    }

    std::tuple<float, float, float, float> PaddingLayout::padding() const
    {
        return std::make_tuple(padLeft_, padTop_, padRight_, padBottom_);
    }

    void PaddingLayout::padding(float padAll)
    {
        padding(padAll, padAll, padAll, padAll);
    }

    void PaddingLayout::padding(float padHorz, float padVert)
    {
        padding(padHorz, padVert, padHorz, padVert);
    }

    void PaddingLayout::padding(float padLeft, float padTop, float padRight, float padBottom)
    {
        padLeft_ = padLeft;
        padTop_ = padTop;
        padRight_ = padRight;
        padBottom_ = padBottom;
        redraw();
    }

    Slot *PaddingLayout::slot()
    {
        return &slot_;
    }
}
