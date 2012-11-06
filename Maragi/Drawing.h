// $Id$

#pragma once

#include "ComUtility.h"
#include "Objects.h"
#include "Singleton.h"

namespace Maragi
{
	namespace UI
	{
		namespace Drawing
		{
			class D2DFactory : public Singleton<D2DFactory>
			{
			private:
				ComPtr<ID2D1Factory> d2dfac;
				ComPtr<IDWriteFactory> dwfac;

			private:
				D2DFactory();
				~D2DFactory();

			public:
				const ComPtr<ID2D1Factory> &getD2DFactory()
				{
					return d2dfac;
				}

				const ComPtr<IDWriteFactory> &getDWriteFactory()
				{
					return dwfac;
				}

				friend class Singleton<D2DFactory>;
			};

			class Context
			{
			private:
				ComPtr<ID2D1HwndRenderTarget> rt;

			public:
				Context();

			public:
				void create(HWND, const Objects::SizeI &);
				void resize(const Objects::SizeI &);
				void beginDraw();
				bool endDraw();

			public:
				operator bool() const
				{
					return rt.get() != nullptr;
				}

				ID2D1HwndRenderTarget *operator ->() const
				{
					return rt;
				}

			private:
				Context(const Context &); // = delete;
			};
		}
	}
}
