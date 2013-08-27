#pragma once

#include "ComUtility.h"
#include "Drawing.h"
#include "Window.h"

namespace Gurigi
{
    class Layout : public Control
    {
    protected:
        Layout(const ControlID &);

    public:
        virtual ControlWeakPtr<> findByPoint(const Objects::PointF &) = 0;
    };

    class ShellLayout : public Layout
    {
    private:
        Slot slot_;

        ComPtr<ID2D1SolidColorBrush> brush;

    protected:
        ShellLayout(const ShellWeakPtr<> &, const ControlID &);
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
        enum { RATIO, REAL } mode;

        GridSize()
            : ratio(0)
            , mode(RATIO)
        {}

        GridSize(int iratio)
            : ratio(static_cast<uint32_t>(iratio))
            , mode(RATIO)
        {}

        GridSize(uint32_t iratio)
            : ratio(iratio)
            , mode(RATIO)
        {}

        GridSize(float irealSize)
            : realSize(irealSize)
            , mode(REAL)
        {}

        GridSize(const GridSize &that)
            : ratio(that.ratio)
            , mode(that.mode)
        {}
    };

    template<size_t rows, size_t cols>
    class GridLayout : public Layout
    {
        static_assert(rows > 0 && cols > 0, "rows and cols are must be greater than 0.");

    private:
        Slot slot_[rows][cols];
        GridSize rowsSize_[rows], colsSize_[cols];
        float widths[cols], heights[rows], widthSubtotal[cols], heightSubtotal[rows];

    protected:
        // TODO: elegant passing array (initializer list will make C++ world elegant...)
        GridLayout(const ControlID &id, const std::vector<GridSize> &rowsSize, const std::vector<GridSize> &colsSize)
            : Layout(id)
        {
            if(rowsSize.size() != rows || colsSize.size() != cols)
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
            ControlPtr<GridLayout> layout(new GridLayout(ControlManager::instance().getNextID(), rowsSize, colsSize));
            for(size_t i = 0; i < rows; ++ i)
            {
                for(size_t j = 0; j < cols; ++ j)
                    layout->slot_[i][j].parent(layout);
            }
            return layout;
        }

    public:
        void createDrawingResources(Drawing::Context &ctx)
        {
            for(size_t i = 0; i < rows; ++ i)
            {
                for(size_t j = 0; j < cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                        lchild->createDrawingResources(ctx);
                }
            }
        }

        void discardDrawingResources(Drawing::Context &ctx)
        {
            for(size_t i = 0; i < rows; ++ i)
            {
                for(size_t j = 0; j < cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                        lchild->discardDrawingResources(ctx);
                }
            }
        }

        virtual void draw(Drawing::Context &ctx)
        {
            for(size_t i = 0; i < rows; ++ i)
            {
                for(size_t j = 0; j < cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                        lchild->draw(ctx);
                }
            }
        }

        virtual Objects::SizeF computeSize()
        {
            float widths[cols] = {0.0f, }, heights[rows] = {0.0f, };
            Objects::SizeF GridSize;
            for(size_t i = 0; i < rows; ++ i)
            {
                for(size_t j = 0; j < cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                    {
                        GridSize = lchild->computeSize();
                        if(widths[j] < GridSize.width)
                            widths[j] = GridSize.width;
                        if(heights[i] < GridSize.height)
                            heights[i] = GridSize.height;
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
            for(size_t i = 0; i < rows; ++ i)
            {
                for(size_t j = 0; j < cols; ++ j)
                {
                    if(Objects::RectangleF(
                        Objects::PointF(widthSubtotal[j], heightSubtotal[i]),
                        Objects::SizeF(widths[j], heights[i])
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

            for(size_t i = 0; i < rows; ++ i)
            {
                for(size_t j = 0; j < cols; ++ j)
                {
                    if(Objects::RectangleF(
                        Objects::PointF(widthSubtotal[j], heightSubtotal[i]),
                        Objects::SizeF(widths[j], heights[i])
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

            for(size_t i = 0; i < rows; ++ i)
            {
                for(size_t j = 0; j < cols; ++ j)
                {
                    if(Objects::RectangleF(
                        Objects::PointF(widthSubtotal[j], heightSubtotal[i]),
                        Objects::SizeF(widths[j], heights[i])
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
            for(size_t i = 0; i < rows; ++ i)
            {
                for(size_t j = 0; j < cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                        lchild->walk(fn);
                }
            }
        }

        void walkReverse(const std::function<void (const ControlWeakPtr<> &)> &fn)
        {
            for(size_t i = 0; i < rows; ++ i)
            {
                for(size_t j = 0; j < cols; ++ j)
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
            if(row >= rows)
                throw(std::out_of_range("row is bigger than allocated rows"));
            else if(col >= cols)
                throw(std::out_of_range("col is bigger than allocated columns"));
            return &slot_[row][col];
        }

        virtual const GridSize &rowSize(size_t row) const
        {
            return rowsSize_[row];
        }

        virtual void rowSize(size_t row, const GridSize &GridSize)
        {
            if(GridSize.mode == GridSize::RATIO && GridSize.ratio == 0)
                throw(std::logic_error("GridSize.ratio must not be zero."));
            rowsSize_[row] = GridSize;
        }

        virtual const GridSize &columnSize(size_t col) const
        {
            return colsSize_[col];
        }

        virtual void columnSize(size_t col, const GridSize &GridSize)
        {
            if(GridSize.mode == GridSize::RATIO && GridSize.ratio == 0)
                throw(std::logic_error("GridSize.ratio must not be zero."));
            colsSize_[col] = GridSize;
        }

    private:
        /*
        Actual grid size is decided by follow process:
        1. set grid sizes which has REAL mode
        2-1. if total ratio is 0, set grid sizes by remain size and computeSize ratios.
        2-2. else, calculate sizes which has ratio = 0 with computeSize, and set remain grid sizes.
        */
        void calculateSizes(const Objects::RectangleF &rect)
        {
            float remainWidth = rect.width(), remainHeight = rect.height();
            float ratio0TotalWidth = 0.0f, ratio0TotalHeight = 0.0f;
            uint32_t totalWidthRatio = 0, totalHeightRatio = 0;
            Objects::SizeF computeSizeCache[rows][cols] = { Objects::SizeF::invalid, };
            size_t i, j;

            widthSubtotal[0] = rect.left;
            heightSubtotal[0] = rect.top;

            for(i = 0; i < rows; ++ i)
            {
                if(rowsSize_[i].mode == GridSize::REAL)
                {
                    heights[i] = rowsSize_[i].realSize;
                    remainHeight -= rowsSize_[i].realSize;
                }
                else
                    totalHeightRatio += rowsSize_[i].ratio;
            }

            for(i = 0; i < cols; ++ i)
            {
                if(colsSize_[i].mode == GridSize::REAL)
                {
                    widths[i] = colsSize_[i].realSize;
                    remainWidth -= colsSize_[i].realSize;
                }
                else
                    totalWidthRatio += colsSize_[i].ratio;
            }

            if(totalHeightRatio == 0)
            {
                for(i = 0; i < rows; ++ i)
                {
                    if(rowsSize_[i].mode == GridSize::RATIO && rowsSize_[i].ratio == 0)
                    {
                        float maxHeight = 0.0f;
                        for(j = 0; j < cols; ++ j)
                        {
                            ControlPtr<> lchild = slot_[i][j].child().lock();
                            if(lchild)
                            {
                                computeSizeCache[i][j] = lchild->rect().size();
                                if(maxHeight < computeSizeCache[i][j].height)
                                    maxHeight = computeSizeCache[i][j].height;
                            }
                        }

                        heights[i] = maxHeight;
                        ratio0TotalHeight += maxHeight;
                    }
                }

                for(i = 0; i < rows; ++ i)
                {
                    if(rowsSize_[i].mode == GridSize::RATIO && rowsSize_[i].ratio == 0)
                        heights[i] = heights[i] * remainHeight / ratio0TotalHeight;
                }
            }
            else
            {
                for(i = 0; i < rows; ++ i)
                {
                    if(rowsSize_[i].mode == GridSize::RATIO && rowsSize_[i].ratio == 0)
                    {
                        float maxHeight = 0.0f;
                        for(j = 0; j < cols; ++ j)
                        {
                            ControlPtr<> lchild = slot_[i][j].child().lock();
                            if(lchild)
                            {
                                computeSizeCache[i][j] = lchild->rect().size();
                                if(maxHeight < computeSizeCache[i][j].height)
                                    maxHeight = computeSizeCache[i][j].height;
                            }
                        }

                        heights[i] = maxHeight;
                        remainHeight -= maxHeight;
                    }
                }

                for(i = 0; i < rows; ++ i)
                {
                    if(rowsSize_[i].mode == GridSize::RATIO && rowsSize_[i].ratio != 0)
                        heights[i] = remainHeight * rowsSize_[i].ratio / totalHeightRatio;
                }
            }

            if(totalWidthRatio == 0)
            {
                for(i = 0; i < cols; ++ i)
                {
                    if(colsSize_[i].mode == GridSize::RATIO && colsSize_[i].ratio == 0)
                    {
                        float maxWidth = 0.0f;
                        for(j = 0; j < rows; ++ j)
                        {
                            if(computeSizeCache[i][j] == Objects::SizeF::invalid)
                            {
                                ControlPtr<> lchild = slot_[i][j].child().lock();
                                if(lchild)
                                    computeSizeCache[i][j] = lchild->rect().size();
                            }

                            if(maxWidth < computeSizeCache[i][j].width)
                                maxWidth = computeSizeCache[i][j].width;
                        }

                        widths[i] = maxWidth;
                        ratio0TotalWidth += maxWidth;
                    }
                }

                for(i = 0; i < cols; ++ i)
                {
                    if(colsSize_[i].mode == GridSize::RATIO && colsSize_[i].ratio == 0)
                        widths[i] = widths[i] * remainWidth / ratio0TotalWidth;
                }
            }
            else
            {
                for(i = 0; i < cols; ++ i)
                {
                    if(colsSize_[i].mode == GridSize::RATIO && colsSize_[i].ratio == 0)
                    {
                        float maxWidth = 0.0f;
                        for(j = 0; j < rows; ++ j)
                        {
                            if(computeSizeCache[i][j] == Objects::SizeF::invalid)
                            {
                                ControlPtr<> lchild = slot_[i][j].child().lock();
                                if(lchild)
                                    computeSizeCache[i][j] = lchild->rect().size();
                            }

                            if(maxWidth < computeSizeCache[i][j].width)
                                maxWidth = computeSizeCache[i][j].width;
                        }

                        widths[i] = maxWidth;
                        remainWidth -= maxWidth;
                    }
                }

                for(i = 0; i < cols; ++ i)
                {
                    if(colsSize_[i].mode == GridSize::RATIO && colsSize_[i].ratio != 0)
                        widths[i] = remainWidth * colsSize_[i].ratio / totalWidthRatio;
                }
            }

            for(i = 1; i < rows; ++ i)
                heightSubtotal[i] = heightSubtotal[i - 1] + heights[i - 1];
            for(i = 1; i < cols; ++ i)
                widthSubtotal[i] = widthSubtotal[i - 1] + widths[i - 1];
        }

    protected:
        virtual void onResizeInternal(const Objects::RectangleF &rect)
        {
            calculateSizes(rect);

            for(size_t i = 0; i < rows; ++ i)
            {
                for(size_t j = 0; j < cols; ++ j)
                {
                    ControlPtr<> lchild = slot_[i][j].child().lock();
                    if(lchild)
                    {
                        lchild->rect(Objects::RectangleF(
                            Objects::PointF(widthSubtotal[j], heightSubtotal[i]),
                            Objects::SizeF(widths[j], heights[i])
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
        PaddingLayout(const ControlID &);
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
