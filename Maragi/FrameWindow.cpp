// $Id$

#include "Common.h"

#include "FrameWindow.h"
#include "Global.h"
#include "UIGlobal.h"
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
				// TODO: set icon message
			}

			Resources::ResourcePtr<Resources::Icon> getIconSmall()
			{
				return self->iconSmall_;
			}

			void setIconSmall(const Resources::ResourcePtr<Resources::Icon> &icon)
			{
				// TODO: set icon message
			}

			Objects::ColorF getBgColor()
			{
				return self->bgColor_;
			}

			void setBgColor(const Objects::ColorF &bgColor)
			{
				self->bgColor_ = bgColor;
				// TODO: invalidate
			}

			Objects::SizeI getClientSize()
			{
				RECT rc;
				GetClientRect(self->hwnd, &rc);
				return Objects::SizeI(rc.right - rc.left, rc.bottom - rc.top);
			}

			void setClientSize(const Objects::SizeI &size)
			{
				// TODO: move window
			}

			Objects::SizeI getWindowSize()
			{
				RECT rc;
				GetClientRect(self->hwnd, &rc);
				return Objects::SizeI(rc.right - rc.left, rc.bottom - rc.top);
			}

			void setWindowSize(const Objects::SizeI &size)
			{
				// TODO: move window
			}

			Objects::PointI getPosition()
			{
				RECT rc;
				GetWindowRect(self->hwnd, &rc);
				return Objects::PointI(rc.left, rc.top);
			}

			void setPosition(const Objects::PointI &pos)
			{
				// TODO: move window
			}

			Objects::SizeI getMinClientSize()
			{
				return self->minClientSize_;
			}

			void setMinClientSize(const Objects::SizeI &size)
			{
				self->minClientSize_ = size;
			}

			Objects::SizeI getMaxClientSize()
			{
				return self->maxClientSize_;
			}

			void setMaxClientSize(const Objects::SizeI &size)
			{
				self->maxClientSize_ = size;
			}
		};

		const uint32_t FrameWindow::windowStyle = WS_OVERLAPPEDWINDOW;
		const uint32_t FrameWindow::windowStyleEx = WS_EX_CLIENTEDGE;

		FrameWindow::FrameWindow()
			: Shell()
			, bgColor_(Objects::ColorF::White)
			, inDestroy(false)
			, capturedButtons(0)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			client.init(impl.get(), &Impl::getClient);
			iconLarge.init(impl.get(), &Impl::getIconLarge, &Impl::setIconLarge);
			iconSmall.init(impl.get(), &Impl::getIconSmall, &Impl::setIconSmall);
			bgColor.init(impl.get(), &Impl::getBgColor, &Impl::setBgColor);
			clientSize.init(impl.get(), &Impl::getClientSize, &Impl::setClientSize);
			windowSize.init(impl.get(), &Impl::getWindowSize, &Impl::setWindowSize);
			position.init(impl.get(), &Impl::getPosition, &Impl::setPosition);
			minClientSize.init(impl.get(), &Impl::getMinClientSize, &Impl::setMinClientSize);
			maxClientSize.init(impl.get(), &Impl::getMaxClientSize, &Impl::setMaxClientSize);
		}

		FrameWindow::FrameWindow(const ShellWeakPtr<> &parent)
			: Shell(parent)
			, bgColor_(Objects::ColorF::White)
			, inDestroy(false)
			, capturedButtons(0)
		{
			impl = std::shared_ptr<Impl>(new Impl(this));
			client.init(impl.get(), &Impl::getClient);
			iconLarge.init(impl.get(), &Impl::getIconLarge, &Impl::setIconLarge);
			iconSmall.init(impl.get(), &Impl::getIconSmall, &Impl::setIconSmall);
			bgColor.init(impl.get(), &Impl::getBgColor, &Impl::setBgColor);
			clientSize.init(impl.get(), &Impl::getClientSize, &Impl::setClientSize);
			windowSize.init(impl.get(), &Impl::getWindowSize, &Impl::setWindowSize);
			position.init(impl.get(), &Impl::getPosition, &Impl::setPosition);
			minClientSize.init(impl.get(), &Impl::getMinClientSize, &Impl::setMinClientSize);
			maxClientSize.init(impl.get(), &Impl::getMaxClientSize, &Impl::setMaxClientSize);
		}

		FrameWindow::~FrameWindow()
		{
			if(hwnd != nullptr && !inDestroy)
				DestroyWindow(hwnd);

			if(!className.empty())
				UnregisterClassW(className.c_str(), Environment::instance().getInstance());
		}

		ShellPtr<FrameWindow> FrameWindow::create(
			const ShellWeakPtr<> &parent,
			const std::wstring &title,
			const Resources::ResourcePtr<Resources::Icon> &iconLarge,
			const Resources::ResourcePtr<Resources::Icon> &iconSmall,
			const Objects::ColorF &bgColor,
			const Objects::SizeI &clientSize,
			const Objects::PointI &position,
			const Objects::SizeI &minClientSize,
			const Objects::SizeI &maxClientSize
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
			frm->bgColor_ = bgColor;
			frm->initPosition = position;
			frm->initClientSize = clientSize;
			frm->minClientSize_ = minClientSize;
			frm->maxClientSize_ = maxClientSize;

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
				// TODO: check this code is correct w/ other DPIs.

				float dpiX, dpiY;
				Drawing::D2DFactory::instance().getD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);
				initClientSize.width = static_cast<int32_t>(ceil(initClientSize.width * dpiX / 96.0f));
				initClientSize.height = static_cast<int32_t>(ceil(initClientSize.height * dpiX / 96.0f));

				initClientSize = adjustWindowSize(initClientSize);

				// XXX: scrollbars?
			}

			ShellPtr<> lparent = parent.get().lock();

			HWND hwnd = CreateWindowExW(
				windowStyleEx,
				className.c_str(),
				initTitle.c_str(),
				windowStyle,
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

		Objects::SizeI FrameWindow::adjustWindowSize(const Objects::SizeI &size)
		{
			RECT rc = { 0, 0, size.width, size.height };
			// TODO: menu
			AdjustWindowRectEx(&rc, windowStyle, FALSE, windowStyleEx);
			return Objects::SizeI(rc.right - rc.left, rc.bottom - rc.top);
		}

		Objects::PointF FrameWindow::screenToClient(const Objects::PointI &pt) const
		{
			// TODO: consider DPI
			POINT wpt = { pt.x, pt.y };
			ScreenToClient(hwnd, &wpt);
			return Objects::PointF(static_cast<float>(wpt.x), static_cast<float>(wpt.y));
		}

		Objects::PointI FrameWindow::clientToScreen(const Objects::PointF &pt) const
		{
			// TODO: consider DPI
			POINT wpt = { static_cast<long>(pt.x), static_cast<long>(pt.y) };
			ClientToScreen(hwnd, &wpt);
			return Objects::PointI(wpt.x, wpt.y);
		}

		ControlWeakPtr<> FrameWindow::hoveredControl(const Objects::PointF &pt) const
		{
			return client_->findByPoint(pt);
		}

		void FrameWindow::redraw()
		{
			InvalidateRect(hwnd, nullptr, FALSE);
		}

		longptr_t FrameWindow::procMessage(HWND hwnd, unsigned message, uintptr_t wParam, longptr_t lParam)
		{
			ControlEventArg ev;
			ev.time = boost::posix_time::microsec_clock::local_time();
			ev.rawMessage = message;
			ev.wParam = wParam;
			ev.lParam = lParam;

			switch(message)
			{
			case WM_SIZE:
				{
					Objects::SizeI size(LOWORD(lParam), HIWORD(lParam));
					context_.resize(size);
					client_->rect = Objects::RectangleF(Objects::PointF(), size);
				}
				return 0;

			case WM_DISPLAYCHANGE:
				InvalidateRect(hwnd, nullptr, FALSE);
				return 0;

			case WM_GETMINMAXINFO:
				{
					MINMAXINFO *mmi = reinterpret_cast<MINMAXINFO *>(lParam);
					if(minClientSize_ != Objects::SizeI::invalid)
					{
						Objects::SizeI min = adjustWindowSize(minClientSize_);
						POINT pt = { min.width, min.height };
						mmi->ptMinTrackSize = pt;
					}
					if(maxClientSize_ != Objects::SizeI::invalid)
					{
						Objects::SizeI max = adjustWindowSize(maxClientSize_);
						POINT pt = { max.width, max.height };
						mmi->ptMaxTrackSize = pt;
					}
				}
				return 0;

			case WM_PAINT:
				if(!context_)
				{
					context_.create(hwnd, clientSize);
					client_->createDrawingResources(context_);
				}

				context_.beginDraw();
				context_->SetTransform(D2D1::Matrix3x2F::Identity());
				context_->Clear(bgColor_);
				client_->draw(context_);
				if(!context_.endDraw())
				{
					client_->discardDrawingResources(context_);
				}

				ValidateRect(hwnd, nullptr);
				return 0;

			case WM_SETCURSOR:
				if(LOWORD(lParam) == HTCLIENT)
				{
					Objects::PointF pt = screenToClient(getCursorPos()); // TODO: get mouse point
					ControlWeakPtr<> hovered = client_->findByPoint(pt);
					ControlPtr<> lhovered = hovered.lock();
					if(lhovered && lhovered->onSetCursorInternal())
						return 1;
				}
				break;

			case WM_MOUSEMOVE:
				{
					Objects::PointF pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
					std::vector<ControlWeakPtr<>> hovereds;
					if(captureds.empty())
						hovereds = client_->findReverseTreeByPoint(pt);
					else
						hovereds = captureds;
					if(!hovereds.empty())
					{
						ev.shellClientPoint = pt;
						if(hovereds.size() != prevHovereds.size()
							|| !std::equal(hovereds.begin(), hovereds.end(), prevHovereds.begin()))
						{
							// mouseout & mouseover
							fireEvent(prevHovereds, &Control::onMouseOut, ev);
							fireEvent(hovereds, &Control::onMouseOver, ev);
						}

						fireEvent(hovereds, &Control::onMouseMove, ev);
						prevHovereds = std::move(hovereds);
					}
				}
				return 0;

			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MBUTTONDBLCLK:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
			case WM_XBUTTONDBLCLK:
				{
					Objects::PointF pt(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
					std::vector<ControlWeakPtr<>> hovereds;
					if(captureds.empty())
						hovereds = client_->findReverseTreeByPoint(pt);
					else
						hovereds = captureds;
					if(!hovereds.empty())
					{
						ev.shellClientPoint = pt;

						switch(message)
						{
						case WM_LBUTTONDOWN:
						case WM_LBUTTONUP:
						case WM_LBUTTONDBLCLK:
							ev.buttonNum = 1;
							break;

						case WM_RBUTTONDOWN:
						case WM_RBUTTONUP:
						case WM_RBUTTONDBLCLK:
							ev.buttonNum = 2;
							break;

						case WM_MBUTTONDOWN:
						case WM_MBUTTONUP:
						case WM_MBUTTONDBLCLK:
							ev.buttonNum = 3;
							break;

						case WM_XBUTTONDOWN:
						case WM_XBUTTONUP:
						case WM_XBUTTONDBLCLK:
							ev.buttonNum = 4; // TODO: X button number
							break;
						}

						switch(message)
						{
						case WM_LBUTTONDOWN:
						case WM_RBUTTONDOWN:
						case WM_MBUTTONDOWN:
						case WM_XBUTTONDOWN:
							if(capturedButtons ++ == 0)
							{
								SetCapture(hwnd_);
								captureds = hovereds;
							}
							fireEvent(hovereds, &Control::onMouseButtonDown, ev);
							break;

						case WM_LBUTTONDBLCLK:
						case WM_RBUTTONDBLCLK:
						case WM_MBUTTONDBLCLK:
						case WM_XBUTTONDBLCLK:
							if(capturedButtons ++ == 0)
							{
								SetCapture(hwnd_);
								captureds = hovereds;
							}
							fireEvent(hovereds, &Control::onMouseButtonDoubleClick, ev);
							break;

						case WM_LBUTTONUP:
						case WM_RBUTTONUP:
						case WM_MBUTTONUP:
						case WM_XBUTTONUP:
							if(-- capturedButtons == 0)
							{
								ReleaseCapture();
								captureds.clear();
							}
							fireEvent(hovereds, &Control::onMouseButtonUp, ev);
							break;
						}
					}
				}
				return 0;

			case WM_DESTROY:
				// TODO: modeless window
				inDestroy = true;
				ShellManager::instance().remove(hwnd);
				PostQuitMessage(0);
				return 0;
			}

			return DefWindowProcW(hwnd, message, wParam, lParam);
		}
	}
}
