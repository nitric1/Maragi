// $Id$

#include "Common.h"

#include "Drawing.h"
#include "Global.h"
#include "Window.h"

namespace Maragi
{
	namespace UI
	{
		namespace Drawing
		{
			D2DFactory::D2DFactory()
			{
				ComInitializer::instance();

				HRESULT hr;

				hr = D2D1CreateFactory(
					D2D1_FACTORY_TYPE_MULTI_THREADED,
					&d2dfac
					);
				if(FAILED(hr))
					throw(ComException("D2D1CreateFactory failed."));

				hr = DWriteCreateFactory(
					DWRITE_FACTORY_TYPE_SHARED,
					__uuidof(IDWriteFactory),
					reinterpret_cast<IUnknown **>(&dwfac)
					);
				if(FAILED(hr))
					throw(ComException("DWriteCreateFactory failed."));
			}

			D2DFactory::~D2DFactory()
			{
			}

			Context::Context()
			{
			}

			void Context::create(HWND hwnd, const Objects::SizeI &size)
			{
				rt.release();
				HRESULT hr;

				hr = D2DFactory::instance().getD2DFactory()->CreateHwndRenderTarget(
					D2D1::RenderTargetProperties(),
					D2D1::HwndRenderTargetProperties(hwnd, size),
					&rt
					);
				if(FAILED(hr))
					throw(ComException("CreateHwndRenderTarget failed."));
			}

			void Context::resize(const Objects::SizeI &size)
			{
				if(rt)
					rt->Resize(size);
			}

			void Context::beginDraw()
			{
				if(rt)
					rt->BeginDraw();
			}

			bool Context::endDraw()
			{
				if(rt)
				{
					HRESULT hr;
					hr = rt->EndDraw();
					if(hr == D2DERR_RECREATE_TARGET)
					{
						rt.release();
						return false;
					}
				}

				return true;
			}
		}
	}
}
