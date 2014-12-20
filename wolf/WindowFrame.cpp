/*!
 * All the regular windows, who take a WNDPROC as a procedure and are created through CreateWindowEx().
 * Part of WOLF - Win32 Object Lambda Framework.
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/wolf
 */

#include "WindowFrame.h"
using namespace wolf;

Frame::~Frame()
{
}

ATOM Frame::Register(const wchar_t *className, int iconId, System::Cursor cursor, System::Color bg)
{
	HINSTANCE hInst = GetModuleHandle(nullptr);
	WNDCLASSEX wc = { 0 };

	wc.cbSize        = sizeof(wc);
	wc.lpfnWndProc   = Frame::_WindowProc;
	wc.hInstance     = hInst;
	wc.lpszClassName = className;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(static_cast<int>(bg) + 1); // http://www.newobjects.com/pages/ndl/alp%5Caf-sysColor.htm
	wc.hCursor       = LoadCursor(nullptr, MAKEINTRESOURCE(cursor));
	wc.style         = CS_DBLCLKS;

	if (iconId) {
		wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(iconId), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR));
		wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(iconId), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	}

	return RegisterClassEx(&wc); // user should register only once, then keep the atom for further calls
}

LRESULT CALLBACK Frame::_WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	Frame *pSelf = nullptr; // in run-time, will be a pointer to the derived-most class
	if (msg == WM_NCCREATE) {
		pSelf = reinterpret_cast<Frame*>((reinterpret_cast<CREATESTRUCT*>(lp))->lpCreateParams); // passed on CreateWindowEx()
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pSelf)); // store pointer to object into HWND room
		*static_cast<Window*>(pSelf) = hwnd; // assign hWnd member
	} else {
		pSelf = reinterpret_cast<Frame*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)); // from HWND room, zero if not set yet
	}

	if (pSelf) {
		_MsgRes r = pSelf->_processMessage(msg, wp, lp);
		if (r.processed) return r.res;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}


Font FramePopup::_hSysFont;

FramePopup::~FramePopup()
{
}

Window FramePopup::createChild(const wchar_t *className, int id, const wchar_t *caption, DWORD exStyle, DWORD style, POINT pos, SIZE size, LPVOID lp)
{
	return Window(CreateWindowEx(exStyle, className, caption, style, pos.x, pos.y, size.cx, size.cy,
		this->hWnd(), reinterpret_cast<HMENU>(id), this->getInstance(), lp));
}

void FramePopup::_internalEvents()
{
	this->onCreate([&]{
		if (!_hSysFont.hFont()) {
			Font::Info nfof = Font::GetDefaultDialogFontInfo();
			_hSysFont.create(nfof);
		}
		_hSysFont.applyOnChildren(this->hWnd());
		this->_setWheelHoverBehavior(); // mousewheel working on cursor hovering, not only when control is focused

		SetFocus(GetNextDlgTabItem(this->hWnd(), nullptr, FALSE)); // focus 1st child according to tab order
		this->sendMessage(WM_INITDIALOG, 0, 0); // can be used to process stuff after the WM_CREATE default processing (font & focus)
	});

	this->onMsg(WM_ACTIVATE, [&](WPARAM wp, LPARAM lp)->LRESULT {
		if (!HIWORD(wp)) { // it not in minimized state
			if (LOWORD(wp) == WA_INACTIVE) {
				this->_hWndCurFocus = GetFocus(); // save currently focused window
			} else {
				SetFocus(this->_hWndCurFocus); // restore focus back
			}
		}
		return 0;
	});

	this->onMsg(WindowPopup::SENDORPOSTMSG, [&](WPARAM wp, LPARAM lp)->LRESULT {
		this->_handleSendOrPostFunction(lp); // for tunelling a callback from another thread, see WindowPopup
		return 0;
	});
}


FrameApp::~FrameApp()
{
}

int FrameApp::run(HINSTANCE hInst, int cmdShow)
{
	this->_internalEvents(); // attach our own events
	this->events(); // attach user events
	if (!_atom) {
		OutputDebugString(L"Loading failed for FrameApp... did you forget to call defineAtom()?\n");
		return -1;
	}

	DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_MINIMIZEBOX |
		(_maximizable ? WS_MAXIMIZEBOX : 0) | (_resizable ? WS_SIZEBOX : WS_BORDER);
	RECT rc = { 0, 0, _size.cx, _size.cy };

	InitCommonControls();
	if (!CreateWindowEx(_acceptDrop ? WS_EX_ACCEPTFILES : 0, // hWnd is set on WM_NCCREATE
		reinterpret_cast<LPCWSTR>(MAKELONG(_atom, 0)),
		_title.c_str(), dwStyle,
		rc.left + GetSystemMetrics(SM_CXSCREEN) / 2 - _size.cx / 2, // center on screen
		rc.top + GetSystemMetrics(SM_CYSCREEN) / 2 - _size.cy / 2,
		rc.right - rc.left, rc.bottom - rc.top,
		nullptr, nullptr, hInst, static_cast<LPVOID>(this)) ) // pass pointer to object
	{
		return -1; // window creation failed
	}

	ShowWindow(this->hWnd(), cmdShow);
	UpdateWindow(this->hWnd());

	MSG msg = { 0 };
	BOOL ret = FALSE;
	while ((ret = GetMessage(&msg, nullptr, 0, 0)) != 0) {
		if (ret == -1) return -1; // failure
		if (_hAccel && TranslateAccelerator(this->hWnd(), _hAccel, &msg)) continue;
		if (IsDialogMessage(this->hWnd(), &msg)) continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam); // this can be used as program return value
}

void FrameApp::_internalEvents()
{
	FramePopup::_internalEvents();

	this->onMsg(WM_DESTROY, [&](WPARAM wp, LPARAM lp)->LRESULT {
		PostQuitMessage(0);
		return 0;
	});
}


FrameModal::~FrameModal()
{
}

int FrameModal::show(WindowPopup *parent)
{
	this->_internalEvents(); // attach our own events
	this->events(); // attach user events
	if (!_atom) {
		OutputDebugString(L"Loading failed for FrameModal... did you forget to call defineAtom()?\n");
		return -1;
	}

	RECT rcP = parent->getWindowRect();
	DWORD dwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_VISIBLE |
		(_maximizable ? WS_MAXIMIZEBOX : 0) | (_resizable ? WS_SIZEBOX : WS_BORDER);
	RECT rc = { 0, 0, _size.cx, _size.cy };
	AdjustWindowRect(&rc, dwStyle, FALSE); // compensate different theme window borders

	CreateWindowEx(WS_EX_DLGMODALFRAME, // hWnd is set on WM_NCCREATE
		reinterpret_cast<LPCWSTR>(MAKELONG(_atom, 0)),
		_title.c_str(), dwStyle,
		(rcP.right - rcP.left) / 2 + rcP.left - _size.cx / 2, // center on parent
		(rcP.bottom - rcP.top) / 2 + rcP.top - _size.cy / 2,
		rc.right - rc.left, rc.bottom - rc.top,
		parent->hWnd(), nullptr, parent->getInstance(),
		static_cast<LPVOID>(this)); // pass pointer to object

	// Parent is turned back active durint WM_CLOSE processing.
	// http://blogs.msdn.com/b/oldnewthing/archive/2005/02/18/376080.aspx
	parent->setEnable(false);

	// A new loop, so caller function is blocked and awaits modal to be closed.
	MSG msg = { 0 };
	while (IsWindow(this->hWnd()) && GetMessage(&msg, nullptr, 0, 0)) {
		if (!(_hAccel && TranslateAccelerator(this->hWnd(), _hAccel, &msg)) && !IsDialogMessage(this->hWnd(), &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return static_cast<int>(msg.wParam);
}

void FrameModal::_internalEvents()
{
	FramePopup::_internalEvents();

	this->onMsg(WM_CLOSE, [&](WPARAM wp, LPARAM lp)->LRESULT {
		this->getParent().setEnable(true); // re-enable parent window
		DestroyWindow(this->hWnd());
		return 0;
	});
}


FrameCtrl::~FrameCtrl()
{
}

void FrameCtrl::create(WindowPopup *parent, POINT position, SIZE size)
{
	this->_internalEvents(); // attach our own events
	this->events(); // attach user events
	if (!_atom) {
		OutputDebugString(L"Loading failed for FrameCtrl... did you forget to call defineAtom()?\n");
		return;
	}

	CreateWindowEx((_border ? WS_EX_CLIENTEDGE : 0) | // http://blogs.msdn.com/b/oldnewthing/archive/2004/07/30/201988.aspx
			(_hasChildren ? WS_EX_CONTROLPARENT : 0),
		reinterpret_cast<LPCWSTR>(MAKELONG(_atom, 0)), nullptr,
		CS_DBLCLKS | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
			static_cast<DWORD>(_scrollBar) | (_tabStop ? (WS_TABSTOP | WS_GROUP) : 0),
		position.x, position.y, size.cx, size.cy,
		parent->hWnd(), reinterpret_cast<HMENU>(_id), parent->getInstance(),
		static_cast<LPVOID>(this)); // pass pointer to object; hWnd is set on WM_NCCREATE
}

void FrameCtrl::_internalEvents()
{
	this->onMsg(WM_NCPAINT, [&](WPARAM wp, LPARAM lp)->LRESULT {
		return this->_drawBorders(wp, lp) ? // themed borders
			0 : DefWindowProc(this->hWnd(), WM_NCPAINT, wp, lp);
	});
}