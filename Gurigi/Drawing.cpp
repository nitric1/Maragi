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
                &d2dfac_
                );
            if(FAILED(hr))
                throw(ComException("D2D1CreateFactory failed."));

            hr = DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory),
                reinterpret_cast<IUnknown **>(&dwfac_));
            if(FAILED(hr))
                throw(ComException("DWriteCreateFactory failed."));
        }

        D2DFactory::~D2DFactory()
        {
        }

        Context::Context()
        {
        }

        void Context::create(HWND hwnd, const Objects::SizeI &size, uint32_t dpi)
        {
            rt_.release();
            HRESULT hr;

            hr = D2DFactory::instance().getD2DFactory()->CreateHwndRenderTarget(
                D2D1::RenderTargetProperties(
                    D2D1_RENDER_TARGET_TYPE_DEFAULT,
                    D2D1::PixelFormat(),
                    static_cast<float>(dpi),
                    static_cast<float>(dpi)
                ),
                D2D1::HwndRenderTargetProperties(hwnd, size),
                &rt_
                );
            if(FAILED(hr))
                throw(ComException("CreateHwndRenderTarget failed."));
        }

        void Context::resize(const Objects::SizeI &size)
        {
            if(rt_)
                rt_->Resize(size);
        }

        void Context::dpi(uint32_t dpi)
        {
            if(rt_)
                rt_->SetDpi(static_cast<float>(dpi), static_cast<float>(dpi));
        }

        void Context::beginDraw()
        {
            if(rt_)
                rt_->BeginDraw();
        }

        bool Context::endDraw()
        {
            if(rt_)
            {
                HRESULT hr;
                hr = rt_->EndDraw();
                if(hr == D2DERR_RECREATE_TARGET)
                {
                    rt_.release();
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
            DWRITE_FONT_STRETCH stretch)
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
