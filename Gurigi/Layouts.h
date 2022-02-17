#pragma once

#include "ComUtility.h"
#include "Drawing.h"
#include "Window.h"

namespace Gurigi
{
    namespace Placer
    {
        struct GridSize
        {
            union
            {
                uint32_t ratio; // ratio 0 represents "fit to control"
                float realSize;
            };
            enum class Mode : uint8_t { Ratio, Real } mode;

            GridSize()
                : ratio(0)
                , mode(Mode::Ratio)
            {}

            GridSize(int iratio)
                : ratio(static_cast<uint32_t>(iratio))
                , mode(Mode::Ratio)
            {}

            GridSize(uint32_t iratio)
                : ratio(iratio)
                , mode(Mode::Ratio)
            {}

            GridSize(float irealSize)
                : realSize(irealSize)
                , mode(Mode::Real)
            {}

            GridSize(const GridSize &that)
                : ratio(that.ratio)
                , mode(that.mode)
            {}
        };

        template<typename PlaceKey = size_t>
        class Placer
        {
        public:
            typedef PlaceKey PlaceKeyT;

        private:
            std::map<PlaceKey, ControlWeakPtr<>> controls_;

        public:
            ControlWeakPtr<> get(PlaceKey placeKey) const
            {
                auto it = controls_.find(placeKey);
                if(it == controls_.end())
                {
                    return nullptr;
                }
                return it->second;
            }
            void set(PlaceKey placeKey, const ControlWeakPtr<> &control)
            {
                auto it = controls_.find(placeKey);
                if(it != controls_.end())
                {
                    controls_.erase(it);
                }

                if(!control.expired())
                {
                    controls_.emplace(placeKey, control);
                }
            }
            const std::map<PlaceKey, ControlWeakPtr<>> &controls() const
            {
                return controls_;
            }

        public:
            virtual void calculateSizes(const Objects::RectangleF &) = 0;
            virtual Objects::RectangleF getRect(PlaceKey) = 0;
        };

        template<size_t Rows, size_t Cols>
        class Grid : public Placer<std::pair<size_t, size_t>>
        {
            static_assert(Rows > 0 && Cols > 0, "rows and cols are must be greater than 0.");

        private:
            std::array<GridSize, Rows> rowsSize_;
            std::array<GridSize, Cols> colsSize_;
            std::array<float, Cols> widths_, widthSubtotal_;
            std::array<float, Rows> heights_, heightSubtotal_;

        public:
            Grid(const std::vector<GridSize> &rowsSize, const std::vector<GridSize> &colsSize)
                : widths_()
                , widthSubtotal_()
                , heights_()
                , heightSubtotal_()
            {
                if(rowsSize.size() != Rows || colsSize.size() != Cols)
                    throw(std::logic_error("GridSize of rowsSize and GridSize of colsSize must match with rows and cols."));
                std::copy_n(std::begin(rowsSize), Rows, std::begin(rowsSize_));
                std::copy_n(std::begin(colsSize), Cols, std::begin(colsSize_));
            }

        public:
            virtual const GridSize &rowSize(size_t row) const
            {
                return rowsSize_[row];
            }

            virtual void rowSize(size_t row, const GridSize &gridSize)
            {
                if(gridSize.mode == GridSize::Mode::Ratio && gridSize.ratio == 0)
                    throw(std::logic_error("GridSize.ratio must not be zero."));
                rowsSize_[row] = gridSize;
            }

            virtual const GridSize &columnSize(size_t col) const
            {
                return colsSize_[col];
            }

            virtual void columnSize(size_t col, const GridSize &gridSize)
            {
                if(gridSize.mode == GridSize::Mode::Ratio && gridSize.ratio == 0)
                    throw(std::logic_error("GridSize.ratio must not be zero."));
                colsSize_[col] = gridSize;
            }

            /*
            Actual grid size is decided by follow process:
            1. set grid sizes which has Real mode
            2-1. if total ratio is 0, set grid sizes by remain size and computeSize ratios.
            2-2. else, calculate sizes which has ratio = 0 with computeSize, and set remain grid sizes.
            */
            virtual void calculateSizes(const Objects::RectangleF &rect)
            {
                float remainWidth = rect.width(), remainHeight = rect.height();
                float ratio0TotalWidth = 0.0f, ratio0TotalHeight = 0.0f;
                uint32_t totalWidthRatio = 0, totalHeightRatio = 0;
                size_t i;

                widths_.fill(0.0f);
                heights_.fill(0.0f);
                widthSubtotal_[0] = rect.left;
                heightSubtotal_[0] = rect.top;

                // calculate reals
                for(i = 0; i < Rows; ++ i)
                {
                    if(rowsSize_[i].mode == GridSize::Mode::Real)
                    {
                        heights_[i] = rowsSize_[i].realSize;
                        remainHeight -= rowsSize_[i].realSize;
                    }
                    else
                        totalHeightRatio += rowsSize_[i].ratio;
                }

                for(i = 0; i < Cols; ++ i)
                {
                    if(colsSize_[i].mode == GridSize::Mode::Real)
                    {
                        widths_[i] = colsSize_[i].realSize;
                        remainWidth -= colsSize_[i].realSize;
                    }
                    else
                        totalWidthRatio += colsSize_[i].ratio;
                }

                // calculate ratios
                auto setHeights = [this](auto &&cb)
                {
                    for(size_t i = 0; i < Rows; ++ i)
                    {
                        if(rowsSize_[i].mode == Gurigi::Placer::GridSize::Mode::Ratio && rowsSize_[i].ratio == 0)
                        {
                            float maxHeight = 0.0f;
                            for(size_t j = 0; j < Cols; ++ j)
                            {
                                auto control = get({i, j});
                                auto lcontrol = control.lock();
                                if(lcontrol)
                                {
                                    auto size = lcontrol->computeSize();
                                    if(maxHeight < size.height)
                                        maxHeight = size.height;
                                }
                                else
                                {
                                    if(maxHeight < 1.0f)
                                        maxHeight = 1.0f;
                                }
                            }

                            heights_[i] = maxHeight;
                            cb(maxHeight);
                        }
                    }
                };

                if(totalHeightRatio == 0)
                {
                    setHeights([&ratio0TotalHeight](float height)
                    {
                        ratio0TotalHeight += height;
                    });

                    for(i = 0; i < Rows; ++ i)
                    {
                        if(rowsSize_[i].mode == GridSize::Mode::Ratio && rowsSize_[i].ratio == 0)
                            heights_[i] = heights_[i] * remainHeight / ratio0TotalHeight;
                    }
                }
                else
                {
                    setHeights([&remainHeight](float height)
                    {
                        remainHeight += height;
                    });

                    for(i = 0; i < Rows; ++ i)
                    {
                        if(rowsSize_[i].mode == GridSize::Mode::Ratio && rowsSize_[i].ratio != 0)
                            heights_[i] = remainHeight * rowsSize_[i].ratio / totalHeightRatio;
                    }
                }

                auto setWidths = [this](auto &&cb)
                {
                    for(size_t i = 0; i < Cols; ++ i)
                    {
                        if(colsSize_[i].mode == Gurigi::Placer::GridSize::Mode::Ratio && colsSize_[i].ratio == 0)
                        {
                            float maxWidth = 0.0f;
                            for(size_t j = 0; j < Rows; ++ j)
                            {
                                auto control = get({j, i});
                                auto lcontrol = control.lock();
                                if(lcontrol)
                                {
                                    auto size = lcontrol->computeSize();
                                    if(maxWidth < size.width)
                                        maxWidth = size.width;
                                }
                                else
                                {
                                    if(maxWidth < 1.0f)
                                        maxWidth = 1.0f;
                                }
                            }

                            widths_[i] = maxWidth;
                            cb(maxWidth);
                        }
                    }
                };

                if(totalWidthRatio == 0)
                {
                    setWidths([&ratio0TotalWidth](float width)
                    {
                        ratio0TotalWidth += width;
                    });

                    for(i = 0; i < Cols; ++ i)
                    {
                        if(colsSize_[i].mode == GridSize::Mode::Ratio && colsSize_[i].ratio == 0)
                            widths_[i] = widths_[i] * remainWidth / ratio0TotalWidth;
                    }
                }
                else
                {
                    setWidths([&remainWidth](float width)
                    {
                        remainWidth -= width;
                    });

                    for(i = 0; i < Cols; ++ i)
                    {
                        if(colsSize_[i].mode == GridSize::Mode::Ratio && colsSize_[i].ratio != 0)
                            widths_[i] = remainWidth * colsSize_[i].ratio / totalWidthRatio;
                    }
                }

                for(i = 1; i < Rows; ++ i)
                    heightSubtotal_[i] = heightSubtotal_[i - 1] + heights_[i - 1];
                for(i = 1; i < Cols; ++ i)
                    widthSubtotal_[i] = widthSubtotal_[i - 1] + widths_[i - 1];
            }

            virtual Objects::RectangleF getRect(std::pair<size_t, size_t> key) override
            {
                return Objects::RectangleF(
                    Objects::PointF(widthSubtotal_[key.second], heightSubtotal_[key.first]),
                    Objects::SizeF(widths_[key.second], heights_[key.first]));
            }

            virtual Objects::RectangleF getRect(size_t row, size_t col)
            {
                return getRect({row, col});
            }
        };
    }

    class Layout : public Control
    {
    protected:
        Layout(const ControlId &);

    public:
        virtual ControlWeakPtr<> findByPoint(const Objects::PointF &) override = 0;

    protected:
        void setParent(const ControlPtr<> &);

    protected:
        static void setParent(const ControlPtr<> &, const ControlWeakPtr<> &);
        static void clearParent(const ControlPtr<> &);
        static void setShell(const ControlPtr<> &, const ShellWeakPtr<> &);

        template<typename>
        friend class EmbeddedLayoutHost;
    };

    class ShellLayout : public Layout
    {
    private:
        ControlPtr<> child_;

        ComPtr<ID2D1SolidColorBrush> brush_;

    protected:
        ShellLayout(const ShellWeakPtr<> &, const ControlId &);
        virtual ~ShellLayout();

    public:
        static ControlPtr<ShellLayout> create(
            const ShellWeakPtr<> &
            );

    public:
        virtual void createDrawingResources(Drawing::Context &) override;
        virtual void discardDrawingResources(Drawing::Context &) override;
        virtual void draw(Drawing::Context &) override;
        virtual Objects::SizeF computeSize() override;
        virtual ControlWeakPtr<> findByPoint(const Objects::PointF &) override;
        virtual std::vector<ControlWeakPtr<>> findTreeByPoint(const Objects::PointF &) override;
        virtual std::vector<ControlWeakPtr<>> findReverseTreeByPoint(const Objects::PointF &) override;
        virtual void walk(const std::function<void (const ControlWeakPtr<> &)> &) override;
        virtual void walkReverse(const std::function<void (const ControlWeakPtr<> &)> &) override;

    public:
        virtual void onResizeInternal(const Objects::RectangleF &) override;

    public:
        using Layout::shell;
        virtual void shell(const ShellWeakPtr<> &) override;

    public:
        virtual void attach(const ControlPtr<> &);
        virtual ControlPtr<> detach();
    };

    using Placer::GridSize;

    template<size_t Rows, size_t Cols>
    class GridLayout : public Layout
    {
        static_assert(Rows > 0 && Cols > 0, "rows and cols are must be greater than 0.");

    private:
        std::array<std::array<ControlPtr<>, Cols>, Rows> children_;
        Placer::Grid<Rows, Cols> placer_;

    protected:
        GridLayout(const ControlId &id, const std::vector<GridSize> &rowsSize, const std::vector<GridSize> &colsSize)
            : Layout(id)
            , placer_(rowsSize, colsSize)
        {}

        virtual ~GridLayout()
        {}

    public:
        static ControlPtr<GridLayout> create(
            const std::vector<GridSize> &rowsSize,
            const std::vector<GridSize> &colsSize
            )
        {
            ControlPtr<GridLayout> layout(new GridLayout(ControlManager::instance().getNextId(), rowsSize, colsSize));
            return layout;
        }

    public:
        virtual void createDrawingResources(Drawing::Context &ctx) override
        {
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(children_[i][j])
                        children_[i][j]->createDrawingResources(ctx);
                }
            }
        }

        virtual void discardDrawingResources(Drawing::Context &ctx) override
        {
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(children_[i][j])
                        children_[i][j]->discardDrawingResources(ctx);
                }
            }
        }

        virtual void draw(Drawing::Context &ctx) override
        {
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(children_[i][j])
                        children_[i][j]->draw(ctx);
                }
            }
        }

        virtual Objects::SizeF computeSize() override
        {
            float widths[Cols] = {0.0f, }, heights[Rows] = {0.0f, };
            Objects::SizeF gridSize;
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(children_[i][j])
                    {
                        gridSize = children_[i][j]->computeSize();
                        if(widths[j] < gridSize.width)
                            widths[j] = gridSize.width;
                        if(heights[i] < gridSize.height)
                            heights[i] = gridSize.height;
                    }
                }
            }
            return Objects::SizeF(
                std::accumulate(std::begin(widths), std::end(widths), 0.0f),
                std::accumulate(std::begin(heights), std::end(heights), 0.0f)
                );
        }

        virtual ControlWeakPtr<> findByPoint(const Objects::PointF &pt) override
        {
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(children_[i][j] && children_[i][j]->rect().isIn(pt))
                    {
                        return children_[i][j]->findByPoint(pt);
                    }
                }
            }
            return nullptr;
        }

        virtual std::vector<ControlWeakPtr<>> findTreeByPoint(const Objects::PointF &pt) override
        {
            if(!rect().isIn(pt))
                return std::vector<ControlWeakPtr<>>();

            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(children_[i][j] && children_[i][j]->rect().isIn(pt))
                    {
                        std::vector<ControlWeakPtr<>> ve = children_[i][j]->findTreeByPoint(pt);
                        ve.insert(ve.begin(), sharedFromThis());
                        return ve;
                    }
                }
            }
            return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
        }

        virtual std::vector<ControlWeakPtr<>> findReverseTreeByPoint(const Objects::PointF &pt) override
        {
            if(!rect().isIn(pt))
                return std::vector<ControlWeakPtr<>>();

            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(children_[i][j] && children_[i][j]->rect().isIn(pt))
                    {
                        std::vector<ControlWeakPtr<>> ve = children_[i][j]->findReverseTreeByPoint(pt);
                        ve.push_back(sharedFromThis());
                        return ve;
                    }
                }
            }
            return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
        }

        virtual void walk(const std::function<void (const ControlWeakPtr<> &)> &fn) override
        {
            fn(sharedFromThis());
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(children_[i][j])
                        children_[i][j]->walk(fn);
                }
            }
        }

        virtual void walkReverse(const std::function<void (const ControlWeakPtr<> &)> &fn) override
        {
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(children_[i][j])
                        children_[i][j]->walkReverse(fn);
                }
            }
            fn(sharedFromThis());
        }

    protected:
        virtual void onResizeInternal(const Objects::RectangleF &rect) override
        {
            placer_.calculateSizes(rect);

            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(children_[i][j])
                        children_[i][j]->rect(placer_.getRect(i, j));
                }
            }
        }

    public:
        using Layout::shell;
        virtual void shell(const ShellWeakPtr<> &shell) override
        {
            Layout::shell(shell);
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(children_[i][j])
                        setShell(children_[i][j], shell);
                }
            }
        }

        virtual const GridSize &rowSize(size_t row) const
        {
            return placer_.rowSize(row);
        }

        virtual void rowSize(size_t row, const GridSize &gridSize)
        {
            placer_.rowSize(row, gridSize);
        }

        virtual const GridSize &columnSize(size_t col) const
        {
            return placer_.columnSize(col);
        }

        virtual void columnSize(size_t col, const GridSize &gridSize)
        {
            placer_.columnSize(col, gridSize);
        }

    public:
        virtual void attach(size_t row, size_t col, const ControlPtr<> &control)
        {
            if(row >= Rows)
                throw(std::out_of_range("row is bigger than allocated rows"));
            else if(col >= Cols)
                throw(std::out_of_range("col is bigger than allocated columns"));
            if(children_[row][col])
                detach(row, col);
            children_[row][col] = control;
            setParent(control);
            placer_.set({row, col}, control);
        }

        virtual ControlPtr<> detach(size_t row, size_t col)
        {
            if(row >= Rows)
                throw(std::out_of_range("row is bigger than allocated rows"));
            else if(col >= Cols)
                throw(std::out_of_range("col is bigger than allocated columns"));
            auto control = children_[row][col];
            if(!control)
                return nullptr;
            children_[row][col] = nullptr;
            placer_.set({row, col}, nullptr);
            clearParent(control);
            return control;
        }
    };

    class PaddingLayout : public Layout
    {
    private:
        ControlPtr<> child_;
        Objects::BoundaryF boundary_;

    protected:
        PaddingLayout(const ControlId &);
        virtual ~PaddingLayout();

    public:
        static ControlPtr<PaddingLayout> create(const Objects::BoundaryF &);

    public:
        virtual void createDrawingResources(Drawing::Context &) override;
        virtual void discardDrawingResources(Drawing::Context &) override;
        virtual void draw(Drawing::Context &) override;
        virtual Objects::SizeF computeSize() override;
        virtual ControlWeakPtr<> findByPoint(const Objects::PointF &) override;
        virtual std::vector<ControlWeakPtr<>> findTreeByPoint(const Objects::PointF &) override;
        virtual std::vector<ControlWeakPtr<>> findReverseTreeByPoint(const Objects::PointF &) override;
        virtual void walk(const std::function<void (const ControlWeakPtr<> &)> &) override;
        virtual void walkReverse(const std::function<void (const ControlWeakPtr<> &)> &) override;

    public:
        virtual void onResizeInternal(const Objects::RectangleF &) override;

    public:
        using Layout::shell;
        virtual void shell(const ShellWeakPtr<> &) override;
        virtual Objects::BoundaryF padding() const;
        virtual void padding(const Objects::BoundaryF &);

    public:
        virtual void attach(const ControlPtr<> &);
        virtual ControlPtr<> detach();
    };

    template<typename Placer>
    class EmbeddedLayoutHost : public Control
    {
    private:
        typedef typename Placer::PlaceKeyT PlaceKey;

    private:
        std::map<PlaceKey, ControlPtr<>> children_;
        Placer &placer_;

    protected:
        EmbeddedLayoutHost(const ControlId &id, Placer &placer)
            : Control(id)
            , placer_(placer)
        {}

    protected:
        virtual void createDrawingResourcesSelf(Drawing::Context &context)
        {
            Control::createDrawingResources(context);
        }
        virtual void discardDrawingResourcesSelf(Drawing::Context &context)
        {
            Control::discardDrawingResources(context);
        }
        virtual void drawSelf(Drawing::Context &) = 0;
        virtual ControlWeakPtr<> findByPointSelf(const Objects::PointF &pt)
        {
            return sharedFromThis();
        }
        virtual std::vector<ControlWeakPtr<>> findTreeByPointSelf(const Objects::PointF &pt)
        {
            return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
        }
        virtual std::vector<ControlWeakPtr<>> findReverseTreeByPointSelf(const Objects::PointF &pt)
        {
            return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
        }
        virtual void walkSelf(const std::function<void(const ControlWeakPtr<> &)> &fn)
        {
            Control::walk(fn);
        }
        virtual void walkReverseSelf(const std::function<void(const ControlWeakPtr<> &)> &fn)
        {
            Control::walkReverse(fn);
        }
        virtual void onResizeInternalSelf(const Objects::RectangleF &rect)
        {
            Control::onResizeInternal(rect);
        }

    private:
        virtual void createDrawingResources(Drawing::Context &context) override final
        {
            createDrawingResourcesSelf(context);
            for(auto &childPair: children_)
            {
                childPair.second->createDrawingResources(context);
            }
        }
        virtual void discardDrawingResources(Drawing::Context &context) override final
        {
            discardDrawingResourcesSelf(context);
            for(auto &childPair: children_)
            {
                childPair.second->discardDrawingResources(context);
            }
        }
        virtual void draw(Drawing::Context &context) override final
        {
            drawSelf(context);
            for(auto &childPair: children_)
            {
                childPair.second->draw(context);
            }
        }
        virtual ControlWeakPtr<> findByPoint(const Objects::PointF &pt) override final
        {
            for(auto &childPair: children_)
            {
                auto control = childPair.second->findByPoint(pt);
                if(!control.expired())
                    return control;
            }
            return findByPointSelf(pt);
        }
        virtual std::vector<ControlWeakPtr<>> findTreeByPoint(const Objects::PointF &pt) override final
        {
            if(!rect().isIn(pt))
                return std::vector<ControlWeakPtr<>>();

            for(auto &childPair: children_)
            {
                std::vector<ControlWeakPtr<>> ve = childPair.second->findTreeByPoint(pt);
                if(!ve.empty())
                    return ve; // excluding self because parent looks there's no "this" on pt.
            }
            return findTreeByPointSelf(pt);
        }
        virtual std::vector<ControlWeakPtr<>> findReverseTreeByPoint(const Objects::PointF &pt) override final
        {
            if(!rect().isIn(pt))
                return std::vector<ControlWeakPtr<>>();

            for(auto &childPair: children_)
            {
                std::vector<ControlWeakPtr<>> ve = childPair.second->findReverseTreeByPoint(pt);
                if(!ve.empty())
                    return ve;
            }
            return findReverseTreeByPointSelf(pt);
        }
        virtual void walk(const std::function<void (const ControlWeakPtr<> &)> &fn) override final
        {
            walkSelf(fn);
            for(auto &childPair: children_)
            {
                childPair.second->walk(fn);
            }
        }
        virtual void walkReverse(const std::function<void (const ControlWeakPtr<> &)> &fn) override final
        {
            for(auto &childPair: children_)
            {
                childPair.second->walkReverse(fn);
            }
            walkReverseSelf(fn);
        }

    public:
        virtual void onResizeInternal(const Objects::RectangleF &rect) override final
        {
            placer_.calculateSizes(rect);
            for(auto &childPair: children_)
            {
                childPair.second->rect(placer_.getRect(childPair.first));
            }
            onResizeInternalSelf(rect);
        }

    protected:
        using Control::shell;
        virtual void shell(const ShellWeakPtr<> &shell) override
        {
            Control::shell(shell);
            for(auto &childPair: children_)
            {
                Layout::setShell(childPair.second, shell);
            }
        }

        void attach(PlaceKey placeKey, const ControlPtr<> &control)
        {
            auto it = children_.find(placeKey);
            if(it != children_.end())
                detach(placeKey);
            children_.emplace(placeKey, control);
            Layout::setParent(control, sharedFromThis());
            placer_.set(placeKey, control);
        }

        ControlPtr<> detach(PlaceKey placeKey)
        {
            auto it = children_.find(placeKey);
            if(it == children_.end())
                return nullptr;
            auto control = it->second;
            children_.erase(it);
            placer_.set(placeKey, nullptr);
            Layout::clearParent(control);
            return control;
        }
    };
}
