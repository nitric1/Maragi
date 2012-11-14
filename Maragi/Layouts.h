// $Id$

#pragma once

#include "ComUtility.h"
#include "Drawing.h"
#include "Window.h"

namespace Maragi
{
	namespace UI
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
			//ShellWeakPtr<> shell_;
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
			//Property::R<ShellLayout, ShellWeakPtr<>> shell;
			Property::R<ShellLayout, Slot *> slot;

		private:
			class Impl;
			friend class Impl;

			std::shared_ptr<Impl> impl;
		};

		template<size_t rows, size_t cols>
		class GridLayout : public Layout
		{
			static_assert(rows > 0 && cols > 0, "rows and cols are must be greater than 0.");

		public:
			struct Size
			{
				union
				{
					uint32_t ratio; // ratio 0 represents remain
					float realSize;
				};
				enum { RATIO, REAL } mode;

				Size()
					: ratio(0)
					, mode(RATIO)
				{}
			};

		private:
			Slot slot_[rows][cols];
			Size rowsSize_[rows], colsSize_[cols];

		protected:
			// TODO: elegant passing array (initializer list will make C++ world elegant...)
			GridLayout(const ControlID &id, const std::vector<Size> &rowsSize, const std::vector<Size> &colsSize)
				: Layout(id)
			{
				if(rowsSize.size() != rows || colsSize.size() != cols)
					throw(std::logic_error("size of rowsSize and size of colsSize must match with rows and cols."));
			}

		public:
			static ControlPtr<GridLayout> create(
				const std::vector<Size> &rowsSize,
				const std::vector<Size> &colsSize
				)
			{
				return new GridLayout(rowsSize, colsSize);
			}

		public:
			virtual void draw(Drawing::Context &ctx)
			{
				for(size_t i = 0; i < rows; ++ i)
				{
					for(size_t j = 0; j < cols; ++ j)
					{
						ControlPtr<> lchild = slot_[i][j].child.get().lock();
						if(lchild)
							lchild->draw(ctx);
					}
				}
			}

			virtual Objects::SizeF computeSize()
			{
				float widths[cols] = {0.0f, }, heights[rows] = {0.0f, };
				Objects::SizeF size;
				for(size_t i = 0; i < rows; ++ i)
				{
					for(size_t j = 0; j < cols; ++ j)
					{
						ControlPtr<> lchild = slot_[i][j].child.get().lock();
						if(lchild)
						{
							size = lchild->computeSize();
							if(widths[j] < size.width)
								widths[j] = size.width;
							if(heights[i] < size.height)
								heights[i] = size.height;
						}
					}
				}
				return Objects::SizeF(
					std::accumulate(std::begin(widths), std::end(widths), 0.0f),
					std::accumulate(std::begin(heights), std::end(heights), 0.0f)
					);
			}

			virtual Slot *operator ()(size_t row, size_t col)
			{
				if(row >= rows_)
					throw(std::out_of_range("row is bigger than allocated rows"));
				else if(col >= cols_)
					throw(std::out_of_range("col is bigger than allocated columns"));
				return &slot_[row][col];
			}

		public:
			virtual void onResizeInternal(const Objects::RectangleF &rect)
			{
				// TODO: calculate childs
			}
		};
	}
}
