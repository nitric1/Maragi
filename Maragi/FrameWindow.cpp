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

			ControlPtr<FullLayout> getClient()
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

			init();
		}

		FrameWindow::FrameWindow(const ShellPtr<Shell> &parent)
			: Shell(parent)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			client.init(impl.get(), &Impl::getClient);
			iconLarge.init(impl.get(), &Impl::getIconLarge, &Impl::setIconLarge);
			iconSmall.init(impl.get(), &Impl::getIconSmall, &Impl::setIconSmall);
			clientSize.init(impl.get(), &Impl::getClientSize, &Impl::setClientSize);

			init();
		}

		FrameWindow::~FrameWindow()
		{
			if(hwnd != nullptr)
				DestroyWindow(hwnd);
		}

		ShellPtr<FrameWindow> FrameWindow::create(
			const ShellPtr<Shell> &parent,
			const std::wstring &title,
			const Resources::ResourcePtr<Resources::Icon> &iconLarge,
			const Resources::ResourcePtr<Resources::Icon> &iconSmall,
			const Objects::PointI &position,
			const Objects::SizeI &clientSize
			)
		{
			ShellPtr<FrameWindow> frm = new FrameWindow(parent);

			frm->iconLarge_ = iconLarge;
			frm->iconSmall_ = iconSmall;

			return frm;
		}

		void FrameWindow::show()
		{
			show(SW_SHOW);
		}

		void FrameWindow::show(int32_t showCommand)
		{
			HWND hwnd = CreateWindowExW(
				0,
				nextID.c_str(),
				title.c_str(),
				WS_OVERLAPPEDWINDOW,
				position.x, position.y,
				clientSize.width, clientSize.height,
				parent ? parent->hwnd.get() : nullptr,
				nullptr,
				Environment::instance().getInstance(),
				&static_cast<ShellPtr<>>(frm)
				);

			hwnd_ = hwnd;
		}

		void FrameWindow::init()
		{
			className = ShellManager::instance().getNextClassName();

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
		}

		longptr_t FrameWindow::procMessage(HWND hwnd, unsigned message, uintptr_t wParam, longptr_t lParam)
		{
			return DefWindowProcW(hwnd, message, wParam, lParam);
		}
	}
}
