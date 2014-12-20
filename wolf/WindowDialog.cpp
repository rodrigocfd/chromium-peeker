/*!
 * All the dialog windows, who take a DLGPROC as a procedure and are created through a dialog resource.
 * Part of WOLF - Win32 Object Lambda Framework.
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/wolf
 */

#include "WindowDialog.h"
using namespace wolf;

Dialog::~Dialog()
{
}

INT_PTR CALLBACK Dialog::_DialogProc(HWND hdlg, UINT msg, WPARAM wp, LPARAM lp)
{
	Dialog *pSelf = nullptr; // in run-time, will be a pointer to the derived-most class
	if (msg == WM_INITDIALOG) {
		pSelf = reinterpret_cast<Dialog*>(lp); // passed on CreateDialogParam() or DialogBoxParam()
		SetWindowLongPtr(hdlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pSelf)); // store pointer to object into HWND room
		*static_cast<Window*>(pSelf) = hdlg; // assign hWnd member
	} else {
		pSelf = reinterpret_cast<Dialog*>(GetWindowLongPtr(hdlg, GWLP_USERDATA)); // from HWND room, zero if not set yet
	}

	if (pSelf) {
		_MsgRes r = pSelf->_processMessage(msg, wp, lp);
		if (r.processed) return r.res;
	}
	return FALSE;
}


Font DialogPopup::_hSysFont;

DialogPopup::~DialogPopup()
{
}

void DialogPopup::_internalEvents()
{
	this->onInitDialog([&]() {
		if (!_hSysFont.hFont()) {
			Font::Info nfof = Font::GetDefaultDialogFontInfo();
			_hSysFont.create(nfof);
		}
		_hSysFont.applyOnChildren(this->hWnd());
		this->_setWheelHoverBehavior(); // mousewheel working on cursor hovering, not only when control is focused
	});

	this->onMsg(WindowPopup::SENDORPOSTMSG, [&](WPARAM wp, LPARAM lp)->INT_PTR {
		this->_handleSendOrPostFunction(lp); // for tunelling a callback from another thread, see WindowPopup
		return 0;
	});
}


DialogApp::~DialogApp()
{
}

int DialogApp::run(HINSTANCE hInst, int cmdShow)
{
	this->_internalEvents(); // attach our own events
	this->events(); // attach user events
	if (!_dialogId) {
		OutputDebugString(L"Loading failed for DialogApp... did you forget to call defineDialog()?\n");
		return -1;
	}

	// The HWND will be assigned during WM_INITDIALOG message, and when the
	// function below returns, it will be already set and ready to be used.
	InitCommonControls();
	CreateDialogParam(hInst, MAKEINTRESOURCE(_dialogId), nullptr,
		Dialog::_DialogProc, reinterpret_cast<LPARAM>(this)); // pass pointer to object

	if (_iconId) {
		this->sendMessage(WM_SETICON, ICON_SMALL,
			reinterpret_cast<LPARAM>(static_cast<HICON>( LoadImage(hInst,
				MAKEINTRESOURCE(_iconId), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR) )));
		this->sendMessage(WM_SETICON, ICON_BIG,
			reinterpret_cast<LPARAM>(static_cast<HICON>( LoadImage(hInst,
				MAKEINTRESOURCE(_iconId), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR) )));
	}

	ShowWindow(this->hWnd(), cmdShow);

	HACCEL hAccel = nullptr; // accelerators table
	if (_accelTableId)
		hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(_accelTableId));

	MSG msg = { 0 };
	BOOL ret = 0;
	while ((ret = GetMessage(&msg, nullptr, 0, 0)) != 0) {
		if (ret == -1) return -1; // failure
		if (hAccel && TranslateAccelerator(this->hWnd(), hAccel, &msg)) continue;
		if (IsDialogMessage(this->hWnd(), &msg)) continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam); // this can be used as program return value
}

void DialogApp::_internalEvents()
{
	DialogPopup::_internalEvents();

	this->onMsg(WM_CLOSE, [&](WPARAM wp, LPARAM lp)->INT_PTR {
		DestroyWindow(this->hWnd());
		return 0;
	});

	this->onMsg(WM_NCDESTROY, [&](WPARAM wp, LPARAM lp)->INT_PTR {
		PostQuitMessage(0);
		return 0;
	});
}


DialogModal::~DialogModal()
{
}

int DialogModal::show(WindowPopup *parent)
{
	this->_internalEvents(); // attach our own events
	this->events(); // attach user events
	if (!_dialogId) {
		OutputDebugString(L"Loading failed for DialogModal... did you forget to call defineDialog()?\n");
		return -1;
	}

	return static_cast<int>( DialogBoxParam(parent->getInstance(),
		MAKEINTRESOURCE(_dialogId), parent->hWnd(), Dialog::_DialogProc,
		reinterpret_cast<LPARAM>(this)) ); // pass pointer to class instance
}

void DialogModal::_internalEvents()
{
	DialogPopup::_internalEvents();

	this->onInitDialog([&]() {
		RECT rcPop = this->getWindowRect();
		RECT rcParent = this->getParent().getWindowRect(); // all relative to screen
		this->setPos(nullptr,
			rcParent.left + (rcParent.right - rcParent.left) / 2 - (rcPop.right - rcPop.left) / 2,
			rcParent.top + (rcParent.bottom - rcParent.top) / 2 - (rcPop.bottom - rcPop.top) / 2,
			0, 0, SWP_NOZORDER | SWP_NOSIZE); // center dialog on parent

		if (_iconId) {
			HINSTANCE hInst = this->getInstance();
			this->sendMessage(WM_SETICON, ICON_SMALL,
				reinterpret_cast<LPARAM>(static_cast<HICON>( LoadImage(hInst,
					MAKEINTRESOURCE(_iconId), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR) )));
			this->sendMessage(WM_SETICON, ICON_BIG,
				reinterpret_cast<LPARAM>(static_cast<HICON>( LoadImage(hInst,
					MAKEINTRESOURCE(_iconId), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR) )));
		} else {
			SetWindowLongPtr(this->hWnd(), GWL_EXSTYLE,
				GetWindowLongPtr(this->hWnd(), GWL_EXSTYLE) | WS_EX_DLGMODALFRAME);
		}
	});

	this->onMsg(WM_CLOSE, [&](WPARAM wp, LPARAM lp)->INT_PTR {
		this->endDialog(IDOK);
		return 0;
	});
}


DialogCtrl::~DialogCtrl()
{
}

void DialogCtrl::create(WindowPopup *parent, POINT position, SIZE size)
{
	this->_internalEvents(); // attach our own events
	this->events(); // attach user events
	if (!_dialogId) {
		OutputDebugString(L"Loading failed for DialogCtrl... did you forget to call defineDialog()?\n");
		return;
	}

	// Dialog styles to be set on the resource editor:
	// - Control: true
	// - Style: child
	// - Visible: true
	CreateDialogParam(parent->getInstance(), MAKEINTRESOURCE(_dialogId), parent->hWnd(),
		Dialog::_DialogProc, reinterpret_cast<LPARAM>(this)); // pass pointer to object
	this->setPos(nullptr, position.x, position.y, size.cx, size.cy, SWP_NOZORDER);
}

void DialogCtrl::_internalEvents()
{
	this->onMsg(WM_NCPAINT, [&](WPARAM wp, LPARAM lp)->INT_PTR {
		return this->_drawBorders(wp, lp) ? // themed borders
			TRUE : FALSE;
	});
}