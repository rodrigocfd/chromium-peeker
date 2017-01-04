/**
 * Part of WinLamb - Win32 API Lambda Library
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/winlamb
 */

#pragma once
#include "base_on_notify.h"

namespace wl {

class base_on final {
private:
	base_inventory& _inventory;
public:
	base_on_notify NOTIFY;

	base_on(base_inventory& inventory) :
		_inventory(inventory), NOTIFY(inventory) { }

	void operator()(UINT msg, std::function<LONG_PTR(params)> func) {
		this->_inventory.add_message(msg, std::move(func));
	}

	void operator()(std::initializer_list<UINT> msgs, std::function<LONG_PTR(params)> func) {
		this->_inventory.add_message(msgs, std::move(func));
	}

	void COMMAND(WORD cmd, std::function<LONG_PTR(params::command)> func) {
		this->_inventory.add_command(cmd, std::move(func));
	}

	void COMMAND(std::initializer_list<WORD> cmds, std::function<LONG_PTR(params::command)> func) {
		this->_inventory.add_command(cmds, std::move(func));
	}


#define MSGDEC(mtype, ptype) \
	void mtype(std::function<LONG_PTR(params::ptype)> func) { \
		this->operator()(WM_##mtype, std::move(func)); \
	}

	MSGDEC(ACTIVATE, activate)
	MSGDEC(ACTIVATEAPP, activateapp)
	MSGDEC(ASKCBFORMATNAME, askcbformatname)
	MSGDEC(CANCELMODE, cancelmode)
	MSGDEC(CAPTURECHANGED, capturechanged)
	MSGDEC(CHANGECBCHAIN, changecbchain)
	MSGDEC(CHAR, char_)
	MSGDEC(CHARTOITEM, chartoitem)
	MSGDEC(CHILDACTIVATE, childactivate)
	MSGDEC(CLOSE, close)
	MSGDEC(COMPACTING, compacting)
	MSGDEC(COMPAREITEM, compareitem)
	MSGDEC(CONTEXTMENU, contextmenu)
	MSGDEC(COPYDATA, copydata)
	MSGDEC(CREATE, create)

	MSGDEC(CTLCOLORBTN, ctlcolorbtn)
	MSGDEC(CTLCOLORDLG, ctlcolordlg)
	MSGDEC(CTLCOLOREDIT, ctlcoloredit)
	MSGDEC(CTLCOLORLISTBOX, ctlcolorlistbox)
	MSGDEC(CTLCOLORSCROLLBAR, ctlcolorscrollbar)
	MSGDEC(CTLCOLORSTATIC, ctlcolorstatic)
	MSGDEC(DEADCHAR, deadchar)
	MSGDEC(DELETEITEM, deleteitem)
	MSGDEC(DESTROY, destroy)
	MSGDEC(DESTROYCLIPBOARD, destroyclipboard)
	MSGDEC(DEVMODECHANGE, devmodechange)
	MSGDEC(DEVICECHANGE, devicechange)
	MSGDEC(DISPLAYCHANGE, displaychange)
	MSGDEC(DRAWCLIPBOARD, drawclipboard)
	MSGDEC(DRAWITEM, drawitem)
	MSGDEC(DROPFILES, dropfiles)
	MSGDEC(ENABLE, enable)
	MSGDEC(ENDSESSION, endsession)
	MSGDEC(ENTERIDLE, enteridle)
	MSGDEC(ENTERMENULOOP, entermenuloop)
	MSGDEC(ENTERSIZEMOVE, entersizemove)
	MSGDEC(ERASEBKGND, erasebkgnd)
	MSGDEC(EXITMENULOOP, exitmenuloop)
	MSGDEC(EXITSIZEMOVE, exitsizemove)
	MSGDEC(FONTCHANGE, fontchange)
	MSGDEC(GETDLGCODE, getdlgcode)
	MSGDEC(GETFONT, getfont)
	MSGDEC(GETHOTKEY, gethotkey)
	MSGDEC(GETICON, geticon)
	MSGDEC(GETMINMAXINFO, getminmaxinfo)
	MSGDEC(GETTEXT, gettext)
	MSGDEC(GETTEXTLENGTH, gettextlength)
	MSGDEC(HELP, help)
	MSGDEC(HOTKEY, hotkey)

	MSGDEC(HSCROLL, hscroll)
	MSGDEC(VSCROLL, vscroll)
	MSGDEC(HSCROLLCLIPBOARD, hscrollclipboard)
	MSGDEC(VSCROLLCLIPBOARD, vscrollclipboard)

	MSGDEC(ICONERASEBKGND, iconerasebkgnd)
	MSGDEC(INITDIALOG, initdialog)
	MSGDEC(INITMENU, initmenu)
	MSGDEC(INITMENUPOPUP, initmenupopup)
	MSGDEC(INPUTLANGCHANGE, inputlangchange)
	MSGDEC(INPUTLANGCHANGEREQUEST, inputlangchangerequest)
	MSGDEC(KEYDOWN, keydown)
	MSGDEC(KEYUP, keyup)
	MSGDEC(KILLFOCUS, killfocus)

	MSGDEC(LBUTTONDBLCLK, lbuttondblclk)
	MSGDEC(LBUTTONDOWN, lbuttondown)
	MSGDEC(LBUTTONUP, lbuttonup)
	MSGDEC(MBUTTONDBLCLK, mbuttondblclk)
	MSGDEC(MBUTTONDOWN, mbuttondown)
	MSGDEC(MBUTTONUP, mbuttonup)
	MSGDEC(MOUSEHOVER, mousehover)
	MSGDEC(MOUSEMOVE, mousemove)
	MSGDEC(RBUTTONDBLCLK, rbuttondblclk)
	MSGDEC(RBUTTONDOWN, rbuttondown)
	MSGDEC(RBUTTONUP, rbuttonup)

	MSGDEC(MDIACTIVATE, mdiactivate)
	MSGDEC(MEASUREITEM, measureitem)
	MSGDEC(MENUCHAR, menuchar)
	MSGDEC(MENUDRAG, menudrag)
	MSGDEC(MENUGETOBJECT, menugetobject)
	MSGDEC(MENURBUTTONUP, menurbuttonup)
	MSGDEC(MENUSELECT, menuselect)
	MSGDEC(MOUSEACTIVATE, mouseactivate)
	MSGDEC(MOUSELEAVE, mouseleave)
	MSGDEC(MOUSEWHEEL, mousewheel)
	MSGDEC(MOVE, move)
	MSGDEC(MOVING, moving)
	MSGDEC(NCACTIVATE, ncactivate)
	MSGDEC(NCCALCSIZE, nccalcsize)
	MSGDEC(NCCREATE, nccreate)
	MSGDEC(NCDESTROY, ncdestroy)
	MSGDEC(NCHITTEST, nchittest)

	MSGDEC(NCLBUTTONDBLCLK, nclbuttondblclk)
	MSGDEC(NCLBUTTONDOWN, nclbuttondown)
	MSGDEC(NCLBUTTONUP, nclbuttonup)
	MSGDEC(NCMBUTTONDBLCLK, ncmbuttondblclk)
	MSGDEC(NCMBUTTONDOWN, ncmbuttondown)
	MSGDEC(NCMBUTTONUP, ncmbuttonup)
	MSGDEC(NCMOUSEMOVE, ncmousemove)
	MSGDEC(NCRBUTTONDBLCLK, ncrbuttondblclk)
	MSGDEC(NCRBUTTONDOWN, ncrbuttondown)
	MSGDEC(NCRBUTTONUP, ncrbuttonup)

	MSGDEC(NCPAINT, ncpaint)
	MSGDEC(NEXTDLGCTL, nextdlgctl)
	MSGDEC(NEXTMENU, nextmenu)
	MSGDEC(NOTIFYFORMAT, notifyformat)
	MSGDEC(PAINT, paint)
	MSGDEC(PAINTCLIPBOARD, paintclipboard)
	MSGDEC(PALETTECHANGED, palettechanged)
	MSGDEC(PALETTEISCHANGING, paletteischanging)
	MSGDEC(PARENTNOTIFY, parentnotify)
	MSGDEC(POWERBROADCAST, powerbroadcast)
	MSGDEC(PRINT, print)
	MSGDEC(PRINTCLIENT, printclient)
	MSGDEC(QUERYDRAGICON, querydragicon)
	MSGDEC(QUERYENDSESSION, queryendsession)
	MSGDEC(QUERYNEWPALETTE, querynewpalette)
	MSGDEC(QUERYOPEN, queryopen)
#define WM_RASDIALEVENT_ WM_RASDIALEVENT
	MSGDEC(RASDIALEVENT_, rasdialevent)
	MSGDEC(RENDERALLFORMATS, renderallformats)
	MSGDEC(RENDERFORMAT, renderformat)
	MSGDEC(SETCURSOR, setcursor)
	MSGDEC(SETFOCUS, setfocus)
	MSGDEC(SETFONT, setfont)
	MSGDEC(SETHOTKEY, sethotkey)
	MSGDEC(SETICON, seticon)
	MSGDEC(SETREDRAW, setredraw)
	MSGDEC(SETTEXT, settext)
	MSGDEC(SETTINGCHANGE, settingchange)
	MSGDEC(SHOWWINDOW, showwindow)
	MSGDEC(SIZE, size)
	MSGDEC(SIZECLIPBOARD, sizeclipboard)
	MSGDEC(SIZING, sizing)
	MSGDEC(SPOOLERSTATUS, spoolerstatus)
	MSGDEC(STYLECHANGED, stylechanged)
	MSGDEC(STYLECHANGING, stylechanging)
	MSGDEC(SYSCHAR, syschar)
	MSGDEC(SYSCOLORCHANGE, syscolorchange)
	MSGDEC(SYSCOMMAND, syscommand)
	MSGDEC(SYSDEADCHAR, sysdeadchar)
	MSGDEC(SYSKEYDOWN, syskeydown)
	MSGDEC(SYSKEYUP, syskeyup)
	MSGDEC(TCARD, tcard)
	MSGDEC(TIMECHANGE, timechange)
	MSGDEC(TIMER, timer)
	MSGDEC(UNINITMENUPOPUP, uninitmenupopup)
	MSGDEC(USERCHANGED, userchanged)
	MSGDEC(VKEYTOITEM, vkeytoitem)
	MSGDEC(WINDOWPOSCHANGED, windowposchanged)
	MSGDEC(WINDOWPOSCHANGING, windowposchanging)
};

}//namespace wl