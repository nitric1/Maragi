#pragma once

#include "ComUtility.h"
#include "Objects.h"
#include "Batang/Singleton.h"

namespace Maragi
{
    namespace UI
    {
        namespace Drawing
        {
            class D2DFactory : public Batang::Singleton<D2DFactory>
            {
            private:
                ComPtr<ID2D1Factory> d2dfac;
                ComPtr<IDWriteFactory> dwfac;

            private:
                D2DFactory();
                ~D2DFactory();

            public:
                const ComPtr<ID2D1Factory> &getD2DFactory() const
                {
                    return d2dfac;
                }

                const ComPtr<IDWriteFactory> &getDWriteFactory() const
                {
                    return dwfac;
                }

                friend class Batang::Singleton<D2DFactory>;
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
                ID2D1HwndRenderTarget *get() const
                {
                    return rt;
                }

                operator bool() const
                {
                    return rt.get() != nullptr;
                }

                ID2D1HwndRenderTarget *operator ->() const
                {
                    return rt;
                }

                operator ID2D1HwndRenderTarget *() const
                {
                    return rt;
                }

            private:
                Context(const Context &); // = delete;
                Context &operator =(const Context &); // = delete;
            };

            class FontFactory : public Batang::Singleton<FontFactory>
            {
            private:
                FontFactory();
                ~FontFactory();

            public:
                ComPtr<IDWriteTextFormat> createFont(
                    float,
                    DWRITE_FONT_WEIGHT = DWRITE_FONT_WEIGHT_REGULAR,
                    DWRITE_TEXT_ALIGNMENT = DWRITE_TEXT_ALIGNMENT_LEADING,
                    DWRITE_PARAGRAPH_ALIGNMENT = DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
                    DWRITE_FONT_STYLE = DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH = DWRITE_FONT_STRETCH_NORMAL
                    );

                friend class Batang::Singleton<FontFactory>;
            };
        }
    }
}
