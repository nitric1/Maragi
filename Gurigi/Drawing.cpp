#include "Common.h"

#include "Drawing.h"
#include "Global.h"
#include "Window.h"

namespace Gurigi
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

        FontFactory::FontFactory()
        {
        }

        FontFactory::~FontFactory()
        {
        }

        ComPtr<IDWriteTextFormat> FontFactory::createFont(
            float size,
            DWRITE_FONT_WEIGHT weight,
            DWRITE_TEXT_ALIGNMENT textAlign,
            DWRITE_PARAGRAPH_ALIGNMENT paraAlign,
            DWRITE_FONT_STYLE style,
            DWRITE_FONT_STRETCH stretch
            )
        {
            const auto &dwfac = D2DFactory::instance().getDWriteFactory();
            ComPtr<IDWriteTextFormat> format;
            HRESULT hr;

            hr = dwfac->CreateTextFormat(
                L"Segoe UI", // TODO: Setting & font fallback
                nullptr,
                weight,
                style,
                stretch,
                floor(size * 10.0f + 0.5f) / 10.0f,
                L"",
                &format
                );
            if(FAILED(hr))
                throw(ComException("CreateTextFormat failed."));

            hr = format->SetTextAlignment(textAlign);
            if(FAILED(hr))
                throw(ComException("SetTextAlignment failed."));

            hr = format->SetParagraphAlignment(paraAlign);
            if(FAILED(hr))
                throw(ComException("SetParagraphAlignment failed."));

            return format;
        }
    }
}
