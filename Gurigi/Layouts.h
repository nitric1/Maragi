#pragma once

#include "ComUtility.h"
#include "Drawing.h"
#include "Window.h"

namespace Gurigi
{
    class Layout : public Control
    {
    protected:
        Layout(const ControlId &);

    public:
        virtual ControlWeakPtr<> findByPoint(const Objects::PointF &) = 0;
    };

    class ShellLayout : public Layout
    {
    private:
        Slot slot_;

        ComPtr<ID2D1SolidColorBrush> brush_;

    protected:
        ShellLayout(const ShellWeakPtr<> &, const ControlId &);
        virtual ~ShellLayout();

    public:
        static ControlPtr<ShellLayout> create(
            const ShellWeakPtr<> &
            );

    public:
        virtual void createDrawingResources(Drawing::Context &);
        virtual void discardDrawingResources(Drawing::Context &);
        virtual void draw(Drawing::Context &);
        virtual Objects::SizeF computeSize();
        virtual ControlWeakPtr<> findByPoint(const Objects::PointF &);
        virtual std::vector<ControlWeakPtr<>> findTreeByPoint(const Objects::PointF &);
        virtual std::vector<ControlWeakPtr<>> findReverseTreeByPoint(const Objects::PointF &);
        virtual void walk(const std::function<void (const ControlWeakPtr<> &)> &);
        virtual void walkReverse(const std::function<void (const ControlWeakPtr<> &)> &);

    public:
        virtual void onResizeInternal(const Objects::RectangleF &);

    public:
        virtual Slot *slot();
    };

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

    template<size_t Rows, size_t Cols>
    class GridLayout : public Layout
    {
        static_assert(Rows > 0 && Cols > 0, "rows and cols are must be greater than 0.");

    private:
        Slot slot_[Rows][Cols];
        GridSize rowsSize_[Rows], colsSize_[Cols];
        float widths_[Cols], heights_[Rows], widthSubtotal_[Cols], heightSubtotal_[Rows];

    protected:
        // TODO: elegant passing array (initializer list will make C++ world elegant...)
        GridLayout(const ControlId &id, const std::vector<GridSize> &rowsSize, const std::vector<GridSize> &colsSize)
            : Layout(id)
        {
            if(rowsSize.size() != Rows || colsSize.size() != Cols)
                throw(std::logic_error("GridSize of rowsSize and GridSize of colsSize must match with rows and cols."));
            std::copy(std::begin(rowsSize), std::end(rowsSize), rowsSize_);
            std::copy(std::begin(colsSize), std::end(colsSize), colsSize_);
        }

        virtual ~GridLayout()
        {}

    public:
        static ControlPtr<GridLayout> create(
            const std::vector<GridSize> &rowsSize,
            const std::vector<GridSize> &colsSize
            )
        {
            ControlPtr<GridLayout> layout(new GridLayout(ControlManager::instance().getNextId(), rowsSize, colsSize));
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                    layout->slot_[i][j].parent(layout);
            }
            return layout;
        }

    public:
        void createDrawingResources(Drawing::Context &ctx)
        {
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                        lchild->createDrawingResources(ctx);
                }
            }
        }

        void discardDrawingResources(Drawing::Context &ctx)
        {
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                        lchild->discardDrawingResources(ctx);
                }
            }
        }

        virtual void draw(Drawing::Context &ctx)
        {
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                        lchild->draw(ctx);
                }
            }
        }

        virtual Objects::SizeF computeSize()
        {
            float widths[Cols] = {0.0f, }, heights[Rows] = {0.0f, };
            Objects::SizeF gridSize;
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                    {
                        gridSize = lchild->computeSize();
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

        virtual ControlWeakPtr<> findByPoint(const Objects::PointF &pt)
        {
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(Objects::RectangleF(
                        Objects::PointF(widthSubtotal_[j], heightSubtotal_[i]),
                        Objects::SizeF(widths_[j], heights_[i])
                        ).isIn(pt))
                    {
                        ControlPtr<> lchild = slot_[i][j].child().lock();
                        if(lchild)
                            return lchild->findByPoint(pt);
                        else
                            break;
                    }
                }
            }
            return nullptr;
        }

        virtual std::vector<ControlWeakPtr<>> findTreeByPoint(const Objects::PointF &pt)
        {
            if(!rect().isIn(pt))
                return std::vector<ControlWeakPtr<>>();

            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(Objects::RectangleF(
                        Objects::PointF(widthSubtotal_[j], heightSubtotal_[i]),
                        Objects::SizeF(widths_[j], heights_[i])
                        ).isIn(pt))
                    {
                        ControlPtr<> lchild = slot_[i][j].child().lock();
                        if(lchild)
                        {
                            std::vector<ControlWeakPtr<>> ve = lchild->findTreeByPoint(pt);
                            ve.insert(ve.begin(), sharedFromThis());
                            return ve;
                        }
                        else
                            break;
                    }
                }
            }
            return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
        }

        virtual std::vector<ControlWeakPtr<>> findReverseTreeByPoint(const Objects::PointF &pt)
        {
            if(!rect().isIn(pt))
                return std::vector<ControlWeakPtr<>>();

            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    if(Objects::RectangleF(
                        Objects::PointF(widthSubtotal_[j], heightSubtotal_[i]),
                        Objects::SizeF(widths_[j], heights_[i])
                        ).isIn(pt))
                    {
                        ControlPtr<> lchild = slot_[i][j].child().lock();
                        if(lchild)
                        {
                            std::vector<ControlWeakPtr<>> ve = lchild->findReverseTreeByPoint(pt);
                            ve.push_back(sharedFromThis());
                            return ve;
                        }
                        else
                            break;
                    }
                }
            }
            return std::vector<ControlWeakPtr<>>(1, sharedFromThis());
        }

        void walk(const std::function<void (const ControlWeakPtr<> &)> &fn)
        {
            fn(sharedFromThis());
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                        lchild->walk(fn);
                }
            }
        }

        void walkReverse(const std::function<void (const ControlWeakPtr<> &)> &fn)
        {
            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                        lchild->walkReverse(fn);
                }
            }
            fn(sharedFromThis());
        }

        virtual Slot *slot(size_t row, size_t col)
        {
            if(row >= Rows)
                throw(std::out_of_range("row is bigger than allocated rows"));
            else if(col >= Cols)
                throw(std::out_of_range("col is bigger than allocated columns"));
            return &slot_[row][col];
        }

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

    private:
        /*
        Actual grid size is decided by follow process:
        1. set grid sizes which has Real mode
        2-1. if total ratio is 0, set grid sizes by remain size and computeSize ratios.
        2-2. else, calculate sizes which has ratio = 0 with computeSize, and set remain grid sizes.
        */
        void calculateSizes(const Objects::RectangleF &rect)
        {
            float remainWidth = rect.width(), remainHeight = rect.height();
            float ratio0TotalWidth = 0.0f, ratio0TotalHeight = 0.0f;
            uint32_t totalWidthRatio = 0, totalHeightRatio = 0;
            Objects::SizeF computeSizeCache[Rows][Cols] = { Objects::SizeF::Invalid, };
            size_t i, j;

            widthSubtotal_[0] = rect.left;
            heightSubtotal_[0] = rect.top;

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

            if(totalHeightRatio == 0)
            {
                for(i = 0; i < Rows; ++ i)
                {
                    if(rowsSize_[i].mode == GridSize::Mode::Ratio && rowsSize_[i].ratio == 0)
                    {
                        float maxHeight = 0.0f;
                        for(j = 0; j < Cols; ++ j)
                        {
                            ControlPtr<> lchild = slot_[i][j].child().lock();
                            if(lchild)
                            {
                                computeSizeCache[i][j] = lchild->rect().size();
                                if(maxHeight < computeSizeCache[i][j].height)
                                    maxHeight = computeSizeCache[i][j].height;
                            }
                        }

                        heights_[i] = maxHeight;
                        ratio0TotalHeight += maxHeight;
                    }
                }

                for(i = 0; i < Rows; ++ i)
                {
                    if(rowsSize_[i].mode == GridSize::Mode::Ratio && rowsSize_[i].ratio == 0)
                        heights_[i] = heights_[i] * remainHeight / ratio0TotalHeight;
                }
            }
            else
            {
                for(i = 0; i < Rows; ++ i)
                {
                    if(rowsSize_[i].mode == GridSize::Mode::Ratio && rowsSize_[i].ratio == 0)
                    {
                        float maxHeight = 0.0f;
                        for(j = 0; j < Cols; ++ j)
                        {
                            ControlPtr<> lchild = slot_[i][j].child().lock();
                            if(lchild)
                            {
                                computeSizeCache[i][j] = lchild->rect().size();
                                if(maxHeight < computeSizeCache[i][j].height)
                                    maxHeight = computeSizeCache[i][j].height;
                            }
                        }

                        heights_[i] = maxHeight;
                        remainHeight -= maxHeight;
                    }
                }

                for(i = 0; i < Rows; ++ i)
                {
                    if(rowsSize_[i].mode == GridSize::Mode::Ratio && rowsSize_[i].ratio != 0)
                        heights_[i] = remainHeight * rowsSize_[i].ratio / totalHeightRatio;
                }
            }

            if(totalWidthRatio == 0)
            {
                for(i = 0; i < Cols; ++ i)
                {
                    if(colsSize_[i].mode == GridSize::Mode::Ratio && colsSize_[i].ratio == 0)
                    {
                        float maxWidth = 0.0f;
                        for(j = 0; j < Rows; ++ j)
                        {
                            if(computeSizeCache[i][j] == Objects::SizeF::Invalid)
                            {
                                ControlPtr<> lchild = slot_[i][j].child().lock();
                                if(lchild)
                                    computeSizeCache[i][j] = lchild->rect().size();
                            }

                            if(maxWidth < computeSizeCache[i][j].width)
                                maxWidth = computeSizeCache[i][j].width;
                        }

                        widths_[i] = maxWidth;
                        ratio0TotalWidth += maxWidth;
                    }
                }

                for(i = 0; i < Cols; ++ i)
                {
                    if(colsSize_[i].mode == GridSize::Mode::Ratio && colsSize_[i].ratio == 0)
                        widths_[i] = widths_[i] * remainWidth / ratio0TotalWidth;
                }
            }
            else
            {
                for(i = 0; i < Cols; ++ i)
                {
                    if(colsSize_[i].mode == GridSize::Mode::Ratio && colsSize_[i].ratio == 0)
                    {
                        float maxWidth = 0.0f;
                        for(j = 0; j < Rows; ++ j)
                        {
                            if(computeSizeCache[i][j] == Objects::SizeF::Invalid)
                            {
                                ControlPtr<> lchild = slot_[i][j].child().lock();
                                if(lchild)
                                    computeSizeCache[i][j] = lchild->rect().size();
                            }

                            if(maxWidth < computeSizeCache[i][j].width)
                                maxWidth = computeSizeCache[i][j].width;
                        }

                        widths_[i] = maxWidth;
                        remainWidth -= maxWidth;
                    }
                }

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

    protected:
        virtual void onResizeInternal(const Objects::RectangleF &rect)
        {
            calculateSizes(rect);

            for(size_t i = 0; i < Rows; ++ i)
            {
                for(size_t j = 0; j < Cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                    {
                        lchild->rect(Objects::RectangleF(
                            Objects::PointF(widthSubtotal_[j], heightSubtotal_[i]),
                            Objects::SizeF(widths_[j], heights_[i])
                            ));
                    }
                }
            }
        }
    };

    class PaddingLayout : public Layout
    {
    private:
        Slot slot_;
        float padLeft_, padTop_, padRight_, padBottom_;

    protected:
        PaddingLayout(const ControlId &);
        virtual ~PaddingLayout();

    public:
        static ControlPtr<PaddingLayout> create(float); // all
        static ControlPtr<PaddingLayout> create(float, float); // lr, tb
        static ControlPtr<PaddingLayout> create(float, float, float, float); // l, t, r, b

    public:
        virtual void createDrawingResources(Drawing::Context &);
        virtual void discardDrawingResources(Drawing::Context &);
        virtual void draw(Drawing::Context &);
        virtual Objects::SizeF computeSize();
        virtual ControlWeakPtr<> findByPoint(const Objects::PointF &);
        virtual std::vector<ControlWeakPtr<>> findTreeByPoint(const Objects::PointF &);
        virtual std::vector<ControlWeakPtr<>> findReverseTreeByPoint(const Objects::PointF &);
        virtual void walk(const std::function<void (const ControlWeakPtr<> &)> &);
        virtual void walkReverse(const std::function<void (const ControlWeakPtr<> &)> &);

    public:
        virtual void onResizeInternal(const Objects::RectangleF &);

    public:
        virtual std::tuple<float, float, float, float> padding() const;
        virtual void padding(float);
        virtual void padding(float, float);
        virtual void padding(float, float, float, float);
        virtual Slot *slot();
    };
}
