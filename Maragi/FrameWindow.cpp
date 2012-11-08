// $Id$

#include "Common.h"

#include "FrameWindow.h"
#include "Global.h"
#include "UIUtility.h"

namespace Maragi
{
	namespace UI
	{
		class FrameWindow::Impl
		{
		private:
			FrameWindow *self;

		public:
			explicit Impl(FrameWindow *iself)
				: self(iself)
			{
			}

			ControlPtr<ShellLayout> getClient()
			{
				return self->client_;
			}

			Resources::ResourcePtr<Resources::Icon> getIconLarge()
			{
				return self->iconLarge_;
			}

			void setIconLarge(const Resources::ResourcePtr<Resources::Icon> &icon)
			{
			}

			Resources::ResourcePtr<Resources::Icon> getIconSmall()
			{
				return self->iconSmall_;
			}

			void setIconSmall(const Resources::ResourcePtr<Resources::Icon> &icon)
			{
			}

			Objects::SizeI getClientSize()
			{
				RECT rc;
				GetClientRect(self->hwnd, &rc);
				return Objects::SizeI(rc.right - rc.left, rc.bottom - rc.top);
			}

			void setClientSize(const Objects::SizeI &size)
			{
			}
		};

		FrameWindow::FrameWindow()
			: Shell()
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			client.init(impl.get(), &Impl::getClient);
			iconLarge.init(impl.get(), &Impl::getIconLarge, &Impl::setIconLarge);
			iconSmall.init(impl.get(), &Impl::getIconSmall, &Impl::setIconSmall);
			clientSize.init(impl.get(), &Impl::getClientSize, &Impl::setClientSize);
		}

		FrameWindow::FrameWindow(const ShellWeakPtr<> &parent)
			: Shell(parent)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			client.init(impl.get(), &Impl::getClient);
			iconLarge.init(impl.get(), &Impl::getIconLarge, &Impl::setIconLarge);
			iconSmall.init(impl.get(), &Impl::getIconSmall, &Impl::setIconSmall);
			clientSize.init(impl.get(), &Impl::getClientSize, &Impl::setClientSize);
		}

		FrameWindow::~FrameWindow()
		{
			if(hwnd != nullptr)
				DestroyWindow(hwnd);

			if(!className.empty())
				UnregisterClassW(className.c_str(), Environment::instance().getInstance());
		}

		ShellPtr<FrameWindow> FrameWindow::create(
			const ShellWeakPtr<> &parent,
			const std::wstring &title,
			const Resources::ResourcePtr<Resources::Icon> &iconLarge,
			const Resources::ResourcePtr<Resources::Icon> &iconSmall,
			const Objects::SizeI &clientSize,
			const Objects::PointI &position
			)
		{
			std::wstring className = ShellManager::instance().getNextClassName();

			WNDCLASSEXW wcex = {0, };
			wcex.cbSize = sizeof(wcex);
			wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra	= sizeof(void *);
			wcex.hInstance = Environment::instance().getInstance();
			wcex.hIcon = *iconLarge;
			wcex.hIconSm = *iconSmall;
			wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
			wcex.hbrBackground = nullptr;
			wcex.lpszMenuName = nullptr;
			wcex.lpszClassName = className.c_str();
			wcex.lpfnWndProc = &ShellManager::windowProc;

			RegisterClassExW(&wcex);

			ShellPtr<FrameWindow> frm = new FrameWindow(parent);

			frm->className = className;
			frm->initTitle = title;
			frm->iconLarge_ = iconLarge;
			frm->iconSmall_ = iconSmall;
			frm->initPosition = position;
			frm->initClientSize = clientSize;

			frm->client_ = ShellLayout::create(frm);

			return frm;
		}

		bool FrameWindow::show()
		{
			return show(SW_SHOW);
		}

		bool FrameWindow::show(int32_t showCommand)
		{
			// TODO: menu

			if(initPosition == initPosition.invalid)
				initPosition = Objects::PointI(CW_USEDEFAULT, CW_USEDEFAULT);
			if(initClientSize == initClientSize.invalid)
				initClientSize = Objects::SizeI(CW_USEDEFAULT, CW_USEDEFAULT);
			else
			{
				float dpiX, dpiY;
				Drawing::D2DFactory::instance().getD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);
				initClientSize.width = static_cast<int>(ceil(initClientSize.width * dpiX / 96.0f));
				initClientSize.height = static_cast<int>(ceil(initClientSize.height * dpiX / 96.0f));

				RECT rc = { 0, 0, initClientSize.width, initClientSize.height };
				AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, FALSE, 0);

				initClientSize.width = rc.right - rc.left;
				initClientSize.height = rc.bottom - rc.top;

				// XXX: scrollbars?
			}

			ShellPtr<> lparent = parent.get().lock();

			HWND hwnd = CreateWindowExW(
				0,
				className.c_str(),
				initTitle.c_str(),
				WS_OVERLAPPEDWINDOW,
				initPosition.x, initPosition.y,
				initClientSize.width, initClientSize.height,
				lparent ? lparent->hwnd.get() : nullptr,
				nullptr,
				Environment::instance().getInstance(),
				&static_cast<ShellWeakPtr<>>(sharedFromThis())
				);
			if(hwnd == nullptr)
				return false;

			hwnd_ = hwnd;
			ShowWindow(hwnd, showCommand);
			UpdateWindow(hwnd);

			MSG msg;
			while(GetMessageW(&msg, nullptr, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}

			return true;
		}

		longptr_t FrameWindow::procMessage(HWND hwnd, unsigned message, uintptr_t wParam, longptr_t lParam)
		{
			switch(message)
			{
			case WM_SIZE:
				{
					Objects::SizeI size(LOWORD(lParam), HIWORD(lParam));
					context_.resize(size);
					client->rect = Objects::RectangleF(Objects::PointF(), size);
				}
				return 0;

			case WM_DISPLAYCHANGE:
				InvalidateRect(hwnd, nullptr, FALSE);
				return 0;

			case WM_PAINT:
				if(!context_)
				{
					context_.create(hwnd, clientSize);
					client->createDrawingResources(context_);
				}

				context_.beginDraw();
				context_->SetTransform(D2D1::Matrix3x2F::Identity());
				context_->Clear(D2D1::ColorF(1.0f, 1.0f, 1.0f));
				client->draw(context_);
				if(!context_.endDraw())
				{
					client->discardDrawingResources(context_);
				}

				ValidateRect(hwnd, nullptr);
				return 0;

			case WM_DESTROY:
				// TODO: modeless window
				ShellManager::instance().remove(hwnd);
				PostQuitMessage(0);
				return 0;
			}

			return DefWindowProcW(hwnd, message, wParam, lParam);
		}
	}
}
