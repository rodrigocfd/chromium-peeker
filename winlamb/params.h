/**	
 * Part of WinLamb - Win32 API Lambda Library
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/winlamb
 */

#pragma once
#include <algorithm> // these 3 for WM_DROPFILES
#include <string>
#include <vector>

#include <Windows.h>
#include <CommCtrl.h>
#include <Dbt.h> // WM_DEVICECHANGE
#include <Ras.h> // WM_RASDIALEVENT

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp)) // from windowsx.h
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

namespace wl {

struct params final {
	UINT   message;
	WPARAM wParam;
	LPARAM lParam;

private:
	struct _break_compat {
		UINT   message;
		WPARAM wParam;
		LPARAM lParam;
		_break_compat(const params& p) :
			message(p.message), wParam(p.wParam), lParam(p.lParam) { }
	};
public:
	struct command;
	struct notify;

	struct activate;
	struct activateapp;
	struct askcbformatname;
	struct cancelmode;
	struct capturechanged;
	struct changecbchain;
	struct char_;
	struct chartoitem;
	struct childactivate;
	struct close;
	struct compacting;
	struct compareitem;
	struct contextmenu;
	struct copydata;
	struct create;

	struct ctlcolorbtn;
	struct ctlcolordlg;
	struct ctlcoloredit;
	struct ctlcolorlistbox;
	struct ctlcolorscrollbar;
	struct ctlcolorstatic;

	struct deadchar;
	struct deleteitem;
	struct destroy;
	struct destroyclipboard;
	struct devmodechange;
	struct devicechange;
	struct displaychange;
	struct drawclipboard;
	struct drawitem;
	struct dropfiles;
	struct enable;
	struct endsession;
	struct enteridle;
	struct entermenuloop;
	struct entersizemove;
	struct erasebkgnd;
	struct exitmenuloop;
	struct exitsizemove;
	struct fontchange;
	struct getdlgcode;
	struct getfont;
	struct gethotkey;
	struct geticon;
	struct getminmaxinfo;
	struct gettext;
	struct gettextlength;
	struct help;
	struct hotkey;

	struct hscroll;
	struct vscroll;
	struct hscrollclipboard;
	struct vscrollclipboard;

	struct iconerasebkgnd;
	struct initdialog;
	struct initmenu;
	struct initmenupopup;
	struct inputlangchange;
	struct inputlangchangerequest;
	struct keydown;
	struct keyup;
	struct killfocus;

	struct lbuttondblclk;
	struct lbuttondown;
	struct lbuttonup;
	struct mbuttondblclk;
	struct mbuttondown;
	struct mbuttonup;
	struct mousehover;
	struct mousemove;
	struct rbuttondblclk;
	struct rbuttondown;
	struct rbuttonup;

	struct mdiactivate;
	struct measureitem;
	struct menuchar;
	struct menudrag;
	struct menugetobject;
	struct menurbuttonup;
	struct menuselect;
	struct mouseactivate;
	struct mouseleave;
	struct mousewheel;
	struct move;
	struct moving;
	struct ncactivate;
	struct nccalcsize;
	struct nccreate;
	struct ncdestroy;
	struct nchittest;

	struct nclbuttondblclk;
	struct nclbuttondown;
	struct nclbuttonup;
	struct ncmbuttondblclk;
	struct ncmbuttondown;
	struct ncmbuttonup;
	struct ncmousemove;
	struct ncrbuttondblclk;
	struct ncrbuttondown;
	struct ncrbuttonup;

	struct ncpaint;
	struct nextdlgctl;
	struct nextmenu;
	struct notifyformat;
	struct paint;
	struct paintclipboard;
	struct palettechanged;
	struct paletteischanging;
	struct parentnotify;
	struct powerbroadcast;
	struct print;
	struct printclient;
	struct querydragicon;
	struct queryendsession;
	struct querynewpalette;
	struct queryopen;
	struct rasdialevent;
	struct renderallformats;
	struct renderformat;
	struct setcursor;
	struct setfocus;
	struct setfont;
	struct sethotkey;
	struct seticon;
	struct setredraw;
	struct settext;
	struct settingchange;
	struct showwindow;
	struct size;
	struct sizeclipboard;
	struct sizing;
	struct spoolerstatus;
	struct stylechanged;
	struct stylechanging;
	struct syschar;
	struct syscolorchange;
	struct syscommand;
	struct sysdeadchar;
	struct syskeydown;
	struct syskeyup;
	struct tcard;
	struct timechange;
	struct timer;
	struct uninitmenupopup;
	struct userchanged;
	struct vkeytoitem;
	struct windowposchanged;
	struct windowposchanging;
};


#define PARMDEC_BEGIN(ptype) \
	struct params::ptype final : public params::_break_compat { \
		ptype(const params& p) : _break_compat(p) { }
#define PARMDEC_END \
	};
#define PARMDEC(ptype) \
	PARMDEC_END \
	PARMDEC_BEGIN(ptype)

PARMDEC_BEGIN(command)
	WORD control_id() const          { return LOWORD(this->wParam); }
	HWND control_hwnd() const        { return reinterpret_cast<HWND>(this->lParam); }
	bool is_from_menu() const        { return HIWORD(this->wParam) == 0; }
	bool is_from_accelerator() const { return HIWORD(this->wParam) == 1; }
PARMDEC(notify)
	NMHDR& hdr() const { return *reinterpret_cast<NMHDR*>(this->lParam); }
PARMDEC_END

PARMDEC_BEGIN(activate)
	bool is_being_activated() const           { return this->wParam != WA_INACTIVE; }
	bool activated_not_by_mouse_click() const { return this->wParam == WA_ACTIVE; }
	bool activated_by_mouse_click() const     { return this->wParam == WA_CLICKACTIVE; }
	HWND swapped_window() const               { return reinterpret_cast<HWND>(this->lParam); }
PARMDEC(activateapp)
	bool  is_being_activated() const { return this->wParam != FALSE; }
	DWORD thread_id() const          { return static_cast<DWORD>(this->lParam); }
PARMDEC(askcbformatname)
	UINT   szbuffer() const { return static_cast<UINT>(this->wParam); }
	TCHAR* buffer() const   { return reinterpret_cast<TCHAR*>(this->lParam); }
PARMDEC(cancelmode)
PARMDEC(capturechanged)
	HWND window_gaining_mouse() const { return reinterpret_cast<HWND>(this->lParam); }
PARMDEC(changecbchain)
	HWND window_being_removed() const { return reinterpret_cast<HWND>(this->wParam); }
	HWND next_window() const          { return reinterpret_cast<HWND>(this->lParam); }
	bool is_last_window() const       { return this->next_window() == nullptr; }
PARMDEC(char_)
	WORD char_code() const           { return static_cast<WORD>(this->wParam); }
	WORD repeat_count() const        { return LOWORD(this->lParam); }
	BYTE scan_code() const           { return LOBYTE(HIWORD(this->lParam)); }
	bool is_extended_key() const     { return (this->lParam >> 24) & 1; }
	bool has_alt_key() const         { return (this->lParam >> 29) & 1; }
	bool key_previously_down() const { return (this->lParam >> 30) & 1; }
	bool key_being_released() const  { return (this->lParam >> 31) & 1; }
PARMDEC(chartoitem)
	WORD char_code() const         { return LOWORD(this->wParam); }
	WORD current_caret_pos() const { return HIWORD(this->wParam); }
	HWND hlistbox() const          { return reinterpret_cast<HWND>(this->lParam); }
PARMDEC(childactivate)
PARMDEC(close)
PARMDEC(compacting)
	UINT cpu_time_ratio() const { return static_cast<UINT>(this->wParam); }
PARMDEC(compareitem)
	WORD               control_id() const        { return static_cast<WORD>(this->wParam); }
	COMPAREITEMSTRUCT& compareitemstruct() const { return *reinterpret_cast<COMPAREITEMSTRUCT*>(this->lParam); }
PARMDEC(contextmenu)
	HWND  target() const { return reinterpret_cast<HWND>(this->wParam); }
	POINT pos() const    { return { GET_X_LPARAM(this->lParam), GET_Y_LPARAM(this->lParam) }; }
PARMDEC(copydata)
	COPYDATASTRUCT& copydatastruct() const { return *reinterpret_cast<COPYDATASTRUCT*>(this->lParam); }
PARMDEC(create)
	CREATESTRUCT& createstruct() const { return *reinterpret_cast<CREATESTRUCT*>(this->lParam); }
PARMDEC_END

#define PARMDEC_CTLCOLOR(ctl) \
	PARMDEC_BEGIN(ctlcolor##ctl) \
		HDC  hdc_##ctl() const { return reinterpret_cast<HDC>(this->wParam); } \
		HWND h##ctl() const    { return reinterpret_cast<HWND>(this->lParam); } \
	PARMDEC_END
	PARMDEC_CTLCOLOR(btn)
	PARMDEC_CTLCOLOR(dlg)
	PARMDEC_CTLCOLOR(edit)
	PARMDEC_CTLCOLOR(listbox)
	PARMDEC_CTLCOLOR(scrollbar)
	PARMDEC_CTLCOLOR(static)

PARMDEC_BEGIN(deadchar)
	WORD char_code() const           { return static_cast<WORD>(this->wParam); }
	WORD repeat_count() const        { return LOWORD(this->lParam); }
	BYTE scan_code() const           { return LOBYTE(HIWORD(this->lParam)); }
	bool is_extended_key() const     { return (this->lParam >> 24) & 1; }
	bool has_alt_key() const         { return (this->lParam >> 29) & 1; }
	bool key_previously_down() const { return (this->lParam >> 30) & 1; }
	bool key_being_released() const  { return (this->lParam >> 31) & 1; }
PARMDEC(deleteitem)
	WORD              control_id() const       { return static_cast<WORD>(this->wParam); }
	DELETEITEMSTRUCT& deleteitemstruct() const { return *reinterpret_cast<DELETEITEMSTRUCT*>(this->lParam); }
PARMDEC(destroy)
PARMDEC(destroyclipboard)
PARMDEC(devmodechange)
	const TCHAR* device_name() const { return reinterpret_cast<const TCHAR*>(this->lParam); }
PARMDEC(devicechange)
	UINT                           event() const                         { return static_cast<UINT>(this->wParam); }
	DEV_BROADCAST_HDR&             dev_broadcast_hdr() const             { return *reinterpret_cast<DEV_BROADCAST_HDR*>(this->lParam); }
	DEV_BROADCAST_DEVICEINTERFACE& dev_broadcast_deviceinterface() const { return *reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE*>(this->lParam); }
	DEV_BROADCAST_HANDLE&          dev_broadcast_handle() const          { return *reinterpret_cast<DEV_BROADCAST_HANDLE*>(this->lParam); }
	DEV_BROADCAST_OEM&             dev_broadcast_oem() const             { return *reinterpret_cast<DEV_BROADCAST_OEM*>(this->lParam); }
	DEV_BROADCAST_PORT&            dev_broadcast_port() const            { return *reinterpret_cast<DEV_BROADCAST_PORT*>(this->lParam); }
	DEV_BROADCAST_VOLUME&          dev_broadcast_volume() const          { return *reinterpret_cast<DEV_BROADCAST_VOLUME*>(this->lParam); }
PARMDEC(displaychange)
	UINT bits_per_pixel() const { return static_cast<UINT>(this->wParam); }
	SIZE sz() const             { return { LOWORD(this->lParam), HIWORD(this->lParam) }; }
PARMDEC(drawclipboard)
PARMDEC(drawitem)
	WORD            control_id() const     { return static_cast<WORD>(this->wParam); }
	bool            is_from_menu() const   { return this->control_id() == 0; }
	DRAWITEMSTRUCT& drawitemstruct() const { return *reinterpret_cast<DRAWITEMSTRUCT*>(this->lParam); }
PARMDEC(dropfiles)
	HDROP hdrop() const { return reinterpret_cast<HDROP>(this->wParam); }
	UINT  count() const { return DragQueryFileW(this->hdrop(), 0xFFFFFFFF, nullptr, 0); }
	std::vector<std::wstring> files() const {
		std::vector<std::wstring> files(this->count()); // alloc return vector
		for (size_t i = 0; i < files.size(); ++i) {
			files[i].resize(DragQueryFileW(this->hdrop(),
				static_cast<UINT>(i), nullptr, 0) + 1, TEXT('\0')); // alloc path string
			DragQueryFileW(this->hdrop(), static_cast<UINT>(i), &files[i][0],
				static_cast<UINT>(files[i].size()));
			files[i].resize(files[i].size() - 1); // trim null
		}
		DragFinish(this->hdrop());
		std::sort(files.begin(), files.end(),
			[](const std::wstring& a, const std::wstring& b)->bool {
			return lstrcmpiW(a.c_str(), b.c_str()) < 0;
		});
		return files;
	}
	POINT pos() const {
		POINT pt = { 0 };
		DragQueryPoint(this->hdrop(), &pt);
		return pt;
	}
PARMDEC(enable)
	bool has_been_enabled() const { return this->wParam != FALSE; }
PARMDEC(endsession)
	bool is_session_being_ended() const { return this->wParam != FALSE; }
	bool is_system_issue() const        { return (this->lParam & ENDSESSION_CLOSEAPP) != 0; }
	bool is_forced_critical() const     { return (this->lParam & ENDSESSION_CRITICAL) != 0; }
	bool is_logoff() const              { return (this->lParam & ENDSESSION_LOGOFF) != 0; }
	bool is_shutdown() const            { return this->lParam == 0; }
PARMDEC(enteridle)
	bool is_menu_displayed() const { return this->wParam == MSGF_MENU; }
	HWND hwindow() const           { return reinterpret_cast<HWND>(this->lParam); }
PARMDEC(entermenuloop)
	bool uses_trackpopupmenu() const { return this->wParam != FALSE; }
PARMDEC(entersizemove)
PARMDEC(erasebkgnd)
	HDC hdc() const { return reinterpret_cast<HDC>(this->wParam); }
PARMDEC(exitmenuloop)
	bool is_shortcut_menu() const { return this->wParam != FALSE; }
PARMDEC(exitsizemove)
PARMDEC(fontchange)
PARMDEC(getdlgcode)
	BYTE vkey_code() const { return static_cast<BYTE>(this->wParam); }
	MSG& msg() const       { return *reinterpret_cast<MSG*>(this->lParam); }
PARMDEC(getfont)
PARMDEC(gethotkey)
PARMDEC(geticon)
	bool is_big() const       { return this->wParam == ICON_BIG; }
	bool is_small() const     { return this->wParam == ICON_SMALL; }
	bool is_small_app() const { return this->wParam == ICON_SMALL2; }
	UINT dpi() const          { return static_cast<UINT>(this->lParam); }
PARMDEC(getminmaxinfo)
	MINMAXINFO& minmaxinfo() const { return *reinterpret_cast<MINMAXINFO*>(this->lParam); }
PARMDEC(gettext)
	UINT   buffer_size() const { return static_cast<UINT>(this->wParam); }
	TCHAR* buffer() const      { return reinterpret_cast<TCHAR*>(this->lParam); }
PARMDEC(gettextlength)
PARMDEC(help)
	HELPINFO& helpinfo() const { return *reinterpret_cast<HELPINFO*>(this->lParam); }
PARMDEC(hotkey)
	bool is_snap_desktop() const { return this->wParam == IDHOT_SNAPDESKTOP; }
	bool is_snap_window() const  { return this->wParam == IDHOT_SNAPWINDOW; }
	bool has_alt() const         { return (LOWORD(this->lParam) & MOD_ALT) != 0; }
	bool has_ctrl() const        { return (LOWORD(this->lParam) & MOD_CONTROL) != 0; }
	bool has_shift() const       { return (LOWORD(this->lParam) & MOD_SHIFT) != 0; }
	bool has_win() const         { return (LOWORD(this->lParam) & MOD_WIN) != 0; }
	BYTE vkey_code() const       { return static_cast<BYTE>(HIWORD(this->lParam)); }
PARMDEC_END

#define PARMDEC_SCROLL(direc) \
	PARMDEC_BEGIN(direc##scroll) \
		WORD scroll_request() const { return LOWORD(this->wParam); } \
		WORD scroll_pos() const     { return HIWORD(this->wParam); } \
		HWND scrollbar() const      { return reinterpret_cast<HWND>(this->lParam); } \
	PARMDEC_END
	PARMDEC_SCROLL(h)
	PARMDEC_SCROLL(v)

#define PARMDEC_SCROLLCLIPBOARD(direc) \
	PARMDEC_BEGIN(direc##scrollclipboard) \
		HWND clipboard_viewer() const { return reinterpret_cast<HWND>(this->wParam); } \
		WORD scroll_event() const     { return LOWORD(this->lParam); } \
		WORD scroll_pos() const       { return HIWORD(this->lParam); } \
	PARMDEC_END
	PARMDEC_SCROLLCLIPBOARD(h)
	PARMDEC_SCROLLCLIPBOARD(v)

PARMDEC_BEGIN(iconerasebkgnd)
	HDC hdc() const { return reinterpret_cast<HDC>(this->wParam); }
PARMDEC(initdialog)
	HWND focused_ctrl() const { return reinterpret_cast<HWND>(this->wParam); }
PARMDEC(initmenu)
	HMENU hmenu() const { return reinterpret_cast<HMENU>(this->wParam); }
PARMDEC(initmenupopup)
	HMENU hmenu() const          { return reinterpret_cast<HMENU>(this->wParam); }
	short relative_pos() const   { return LOWORD(this->lParam); }
	bool  is_window_menu() const { return HIWORD(this->lParam) != FALSE; }
PARMDEC(inputlangchange)
	DWORD new_charset() const     { return static_cast<DWORD>(this->wParam); }
	HKL   keyboard_layout() const { return reinterpret_cast<HKL>(this->lParam); }
PARMDEC(inputlangchangerequest)
	bool previous_chosen() const      { return (this->wParam & INPUTLANGCHANGE_BACKWARD) != 0; }
	bool next_chosen() const          { return (this->wParam & INPUTLANGCHANGE_FORWARD) != 0; }
	bool can_be_used_with_sys() const { return (this->wParam & INPUTLANGCHANGE_SYSCHARSET) != 0; }
	HKL  keyboard_layout() const      { return reinterpret_cast<HKL>(this->lParam); }
PARMDEC(keydown)
	BYTE vkey_code() const           { return static_cast<BYTE>(this->wParam); }
	WORD repeat_count() const        { return LOWORD(this->lParam); }
	BYTE scan_code() const           { return LOBYTE(HIWORD(this->lParam)); }
	bool is_extended_key() const     { return (this->lParam >> 24) & 1; }
	bool key_previously_down() const { return (this->lParam >> 30) & 1; }
PARMDEC(keyup)
	BYTE vkey_code() const       { return static_cast<BYTE>(this->wParam); }
	BYTE scan_code() const       { return LOBYTE(HIWORD(this->lParam)); }
	bool is_extended_key() const { return (this->lParam >> 24) & 1; }
PARMDEC(killfocus)
	HWND focused_window() const { return reinterpret_cast<HWND>(this->wParam); }
PARMDEC_END

#define PARMDEC_MOUSE(btnmsg) \
	PARMDEC_BEGIN(btnmsg) \
		bool  has_ctrl() const       { return (this->wParam & MK_CONTROL) != 0; } \
		bool  has_left_btn() const   { return (this->wParam & MK_LBUTTON) != 0; } \
		bool  has_middle_btn() const { return (this->wParam & MK_MBUTTON) != 0; } \
		bool  has_right_btn() const  { return (this->wParam & MK_RBUTTON) != 0; } \
		bool  has_shift() const      { return (this->wParam & MK_SHIFT) != 0; } \
		bool  has_xbtn1() const      { return (this->wParam & MK_XBUTTON1) != 0; } \
		bool  has_xbtn2() const      { return (this->wParam & MK_XBUTTON2) != 0; } \
		POINT pos() const            { return { GET_X_LPARAM(this->lParam), GET_Y_LPARAM(this->lParam) }; } \
	PARMDEC_END
	PARMDEC_MOUSE(lbuttondblclk)
	PARMDEC_MOUSE(lbuttondown)
	PARMDEC_MOUSE(lbuttonup)
	PARMDEC_MOUSE(mbuttondblclk)
	PARMDEC_MOUSE(mbuttondown)
	PARMDEC_MOUSE(mbuttonup)
	PARMDEC_MOUSE(mousehover)
	PARMDEC_MOUSE(mousemove)
	PARMDEC_MOUSE(rbuttondblclk)
	PARMDEC_MOUSE(rbuttondown)
	PARMDEC_MOUSE(rbuttonup)

PARMDEC_BEGIN(mdiactivate)
	HWND activated_child() const   { return reinterpret_cast<HWND>(this->wParam); }
	HWND deactivated_child() const { return reinterpret_cast<HWND>(this->lParam); }
PARMDEC(measureitem)
	MEASUREITEMSTRUCT& measureitemstruct() const { return *reinterpret_cast<MEASUREITEMSTRUCT*>(this->lParam); }
PARMDEC(menuchar)
	WORD  char_code() const      { return LOWORD(this->wParam); }
	bool  is_window_menu() const { return HIWORD(this->wParam) == MF_SYSMENU; }
	HMENU hmenu() const          { return reinterpret_cast<HMENU>(this->lParam); }
PARMDEC(menudrag)
	UINT  initial_pos() const { static_cast<UINT>(this->wParam); }
	HMENU hmenu() const       { return reinterpret_cast<HMENU>(this->lParam); }
PARMDEC(menugetobject)
	MENUGETOBJECTINFO& menugetobjectinfo() const { return *reinterpret_cast<MENUGETOBJECTINFO*>(this->lParam); }
PARMDEC(menurbuttonup)
	UINT  index() const { return static_cast<UINT>(this->wParam); }
	HMENU hmenu() const { return reinterpret_cast<HMENU>(this->lParam); }
PARMDEC(menuselect)
	WORD  item() const              { return LOWORD(this->wParam); }
	bool  has_bitmap() const        { return (HIWORD(this->wParam) & MF_BITMAP) != 0; }
	bool  is_checked() const        { return (HIWORD(this->wParam) & MF_CHECKED) != 0; }
	bool  is_disabled() const       { return (HIWORD(this->wParam) & MF_DISABLED) != 0; }
	bool  is_grayed() const         { return (HIWORD(this->wParam) & MF_GRAYED) != 0; }
	bool  is_highlighted() const    { return (HIWORD(this->wParam) & MF_HILITE) != 0; }
	bool  mouse_selected() const    { return (HIWORD(this->wParam) & MF_MOUSESELECT) != 0; }
	bool  is_owner_draw() const     { return (HIWORD(this->wParam) & MF_OWNERDRAW) != 0; }
	bool  opens_popup() const       { return (HIWORD(this->wParam) & MF_POPUP) != 0; }
	bool  is_sysmenu() const        { return (HIWORD(this->wParam) & MF_SYSMENU) != 0; }
	bool  system_has_closed() const { return HIWORD(this->wParam) == 0xFFFF && !this->lParam; }
	HMENU hmenu() const             { return (this->opens_popup() || this->is_sysmenu()) ? reinterpret_cast<HMENU>(this->lParam) : nullptr; }
PARMDEC(mouseactivate)
	short hit_test_code() const { return static_cast<short>(LOWORD(this->lParam)); }
	WORD  mouse_msg_id() const  { return HIWORD(this->lParam); }
PARMDEC(mouseleave)
PARMDEC(mousewheel)
	short wheel_delta() const    { return GET_WHEEL_DELTA_WPARAM(this->wParam); }
	bool  has_ctrl() const       { return (LOWORD(this->wParam) & MK_CONTROL) != 0; }
	bool  has_left_btn() const   { return (LOWORD(this->wParam) & MK_LBUTTON) != 0; }
	bool  has_middle_btn() const { return (LOWORD(this->wParam) & MK_MBUTTON) != 0; }
	bool  has_right_btn() const  { return (LOWORD(this->wParam) & MK_RBUTTON) != 0; }
	bool  has_shift() const      { return (LOWORD(this->wParam) & MK_SHIFT) != 0; }
	bool  has_xbtn1() const      { return (LOWORD(this->wParam) & MK_XBUTTON1) != 0; }
	bool  has_xbtn2() const      { return (LOWORD(this->wParam) & MK_XBUTTON2) != 0; }
	POINT pos() const            { return { GET_X_LPARAM(this->lParam), GET_Y_LPARAM(this->lParam) }; }
PARMDEC(move)
	POINT pos() const { return { GET_X_LPARAM(this->lParam), GET_Y_LPARAM(this->lParam) }; }
PARMDEC(moving)
	RECT& screen_coords() const { return *reinterpret_cast<RECT*>(this->lParam); }
PARMDEC(ncactivate)
	bool is_active() const { return this->wParam == TRUE; }
PARMDEC(nccalcsize)
	bool               is_nccalcsize() const     { return this->wParam == TRUE; }
	bool               is_rect() const           { return this->wParam == FALSE; }
	NCCALCSIZE_PARAMS& nccalcsize_params() const { return *reinterpret_cast<NCCALCSIZE_PARAMS*>(this->lParam); }
	RECT&              rect() const              { return *reinterpret_cast<RECT*>(this->lParam); }
PARMDEC(nccreate)
	CREATESTRUCT& createstruct() const { return *reinterpret_cast<CREATESTRUCT*>(this->lParam); }
PARMDEC(ncdestroy)
PARMDEC(nchittest)
	POINT pos() const { return { GET_X_LPARAM(this->lParam), GET_Y_LPARAM(this->lParam) }; }
PARMDEC_END

#define PARMDEC_NCBUTTON(ncbtnmsg) \
	PARMDEC_BEGIN(ncbtnmsg) \
		short hit_test_code() const { return static_cast<short>(this->wParam); } \
		POINT pos() const           { return { GET_X_LPARAM(this->lParam), GET_Y_LPARAM(this->lParam) }; } \
	PARMDEC_END
	PARMDEC_NCBUTTON(nclbuttondblclk);
	PARMDEC_NCBUTTON(nclbuttondown);
	PARMDEC_NCBUTTON(nclbuttonup);
	PARMDEC_NCBUTTON(ncmbuttondblclk);
	PARMDEC_NCBUTTON(ncmbuttondown);
	PARMDEC_NCBUTTON(ncmbuttonup);
	PARMDEC_NCBUTTON(ncmousemove);
	PARMDEC_NCBUTTON(ncrbuttondblclk);
	PARMDEC_NCBUTTON(ncrbuttondown);
	PARMDEC_NCBUTTON(ncrbuttonup);

PARMDEC_BEGIN(ncpaint)
	HRGN hrgn() const { return reinterpret_cast<HRGN>(this->wParam); }
PARMDEC(nextdlgctl)
	bool has_ctrl_receiving_focus() const { return LOWORD(this->lParam) != FALSE; }
	HWND ctrl_receiving_focus() const     { return LOWORD(this->lParam) ? reinterpret_cast<HWND>(this->wParam) : nullptr; }
	bool focus_next() const               { return this->wParam == 0; }
PARMDEC(nextmenu)
	BYTE         vkey_code() const   { return static_cast<BYTE>(this->wParam); }
	MDINEXTMENU& mdinextmenu() const { return *reinterpret_cast<MDINEXTMENU*>(this->lParam); }
PARMDEC(notifyformat)
	HWND window_from() const           { return reinterpret_cast<HWND>(this->wParam); }
	bool is_query_from_control() const { return this->lParam == NF_QUERY; }
	bool is_requery_to_control() const { return this->lParam == NF_REQUERY; }
PARMDEC(paint)
PARMDEC(paintclipboard)
	HWND               clipboard_viewer() const { return reinterpret_cast<HWND>(this->wParam); }
	const PAINTSTRUCT& paintstruct() const      { return *reinterpret_cast<const PAINTSTRUCT*>(this->lParam); }
PARMDEC(palettechanged)
	HWND window_origin() const { return reinterpret_cast<HWND>(this->wParam); }
PARMDEC(paletteischanging)
	HWND window_origin() const { return reinterpret_cast<HWND>(this->wParam); }
PARMDEC(parentnotify)
	UINT  event_message() const { return static_cast<UINT>(LOWORD(this->wParam)); }
	WORD  child_id() const      { return HIWORD(this->wParam); }
	HWND  child_hwnd() const    { return reinterpret_cast<HWND>(this->lParam); }
	POINT pos() const           { return { GET_X_LPARAM(this->lParam), GET_Y_LPARAM(this->lParam) }; }
	bool  is_xbutton1() const   { return HIWORD(this->wParam) == XBUTTON1; }
	WORD  pointer_flag() const  { return HIWORD(this->wParam); }
PARMDEC(powerbroadcast)
	bool                    is_power_status_change() const  { return this->wParam == PBT_APMPOWERSTATUSCHANGE; }
	bool                    is_resuming() const             { return this->wParam == PBT_APMRESUMEAUTOMATIC; }
	bool                    is_suspending() const           { return this->wParam == PBT_APMSUSPEND; }
	bool                    is_power_setting_change() const { return this->wParam == PBT_POWERSETTINGCHANGE; }
	POWERBROADCAST_SETTING& power_setting() const           { return *reinterpret_cast<POWERBROADCAST_SETTING*>(this->lParam); }
PARMDEC(print)
	HDC  hdc() const   { return reinterpret_cast<HDC>(this->wParam); }
	UINT flags() const { return static_cast<UINT>(this->lParam); }
PARMDEC(printclient)
	HDC  hdc() const   { return reinterpret_cast<HDC>(this->wParam); }
	UINT flags() const { return static_cast<UINT>(this->lParam); }
PARMDEC(querydragicon)
PARMDEC(queryendsession)
	bool is_system_issue() const    { return (this->lParam & ENDSESSION_CLOSEAPP) != 0; }
	bool is_forced_critical() const { return (this->lParam & ENDSESSION_CRITICAL) != 0; }
	bool is_logoff() const          { return (this->lParam & ENDSESSION_LOGOFF) != 0; }
	bool is_shutdown() const        { return this->lParam == 0; }
PARMDEC(querynewpalette)
PARMDEC(queryopen)
PARMDEC(rasdialevent)
	RASCONNSTATE rasconnstate() const { return static_cast<RASCONNSTATE>(this->wParam); }
	DWORD        error() const        { return static_cast<DWORD>(this->lParam); }
PARMDEC(renderallformats)
PARMDEC(renderformat)
	WORD clipboard_format() const { return static_cast<WORD>(this->wParam); }
PARMDEC(setcursor)
	HWND  cursor_owner() const  { return reinterpret_cast<HWND>(this->wParam); }
	short hit_test_code() const { return static_cast<short>(LOWORD(this->wParam)); }
	WORD  mouse_msg_id() const  { return HIWORD(this->wParam); }
PARMDEC(setfocus)
	HWND unfocused_window() const { return reinterpret_cast<HWND>(this->wParam); }
PARMDEC(setfont)
	HFONT hfont() const         { return reinterpret_cast<HFONT>(this->wParam); }
	bool  should_redraw() const { return LOWORD(this->lParam) != FALSE; }
PARMDEC(sethotkey)
	BYTE vkey_code() const    { return static_cast<BYTE>(LOWORD(this->wParam)); }
	bool has_alt() const      { return (HIWORD(this->wParam) & HOTKEYF_ALT) != 0; }
	bool has_ctrl() const     { return (HIWORD(this->wParam) & HOTKEYF_CONTROL) != 0; }
	bool has_extended() const { return (HIWORD(this->wParam) & HOTKEYF_EXT) != 0; }
	bool has_shift() const    { return (HIWORD(this->wParam) & HOTKEYF_SHIFT) != 0; }
PARMDEC(seticon)
	bool  is_small() const   { return this->wParam == ICON_SMALL; }
	HICON hicon() const      { return reinterpret_cast<HICON>(this->lParam); }
	bool  is_removed() const { return this->hicon() == nullptr; }
PARMDEC(setredraw)
	bool can_redraw() const { return this->wParam != FALSE; }
PARMDEC(settext)
	const TCHAR* text() const { return reinterpret_cast<const TCHAR*>(this->lParam); }
PARMDEC(settingchange)
	const TCHAR* string_id() const           { return reinterpret_cast<const TCHAR*>(this->lParam); }
	bool         is_policy() const           { return !lstrcmp(this->string_id(), TEXT("Policy")); }
	bool         is_locale() const           { return !lstrcmp(this->string_id(), TEXT("intl")); }
	bool         is_environment_vars() const { return !lstrcmp(this->string_id(), TEXT("Environment")); }
PARMDEC(showwindow)
	bool is_being_shown() const           { return this->wParam != FALSE; }
	bool is_other_away() const            { return this->lParam == SW_OTHERUNZOOM; }
	bool is_other_over() const            { return this->lParam == SW_OTHERZOOM; }
	bool is_owner_being_minimized() const { return this->lParam == SW_PARENTCLOSING; }
	bool is_owner_being_restored() const  { return this->lParam == SW_PARENTOPENING; }
PARMDEC(size)
	bool is_other_maximized() const { return this->wParam == 4; }
	bool is_maximized() const       { return this->wParam == 2; }
	bool is_other_restored() const  { return this->wParam == 3; }
	bool is_minimized() const       { return this->wParam == 1; }
	bool is_restored() const        { return this->wParam == 0; }
	SIZE sz() const                 { return { LOWORD(this->lParam), HIWORD(this->lParam) }; }
PARMDEC(sizeclipboard)
	HWND        clipboard_viewer() const { return reinterpret_cast<HWND>(this->wParam); }
	const RECT& clipboard_rect() const   { return *reinterpret_cast<const RECT*>(this->lParam); }
PARMDEC(sizing)
	WORD  edge() const          { return static_cast<WORD>(this->wParam); }
	RECT& screen_coords() const { *reinterpret_cast<RECT*>(this->lParam); }
PARMDEC(spoolerstatus)
	UINT status_flag() const    { return static_cast<UINT>(this->wParam); }
	WORD remaining_jobs() const { return LOWORD(this->lParam); }
PARMDEC(stylechanged)
	bool               is_style() const    { return (this->wParam & GWL_STYLE) != 0; }
	bool               is_ex_style() const { return (this->wParam & GWL_EXSTYLE) != 0; }
	const STYLESTRUCT& stylestruct() const { return *reinterpret_cast<const STYLESTRUCT*>(this->lParam); }
PARMDEC(stylechanging)
	bool               is_style() const    { return (this->wParam & GWL_STYLE) != 0; }
	bool               is_ex_style() const { return (this->wParam & GWL_EXSTYLE) != 0; }
	const STYLESTRUCT& stylestruct() const { return *reinterpret_cast<const STYLESTRUCT*>(this->lParam); }
PARMDEC(syschar)
	WORD char_code() const           { return static_cast<WORD>(this->wParam); }
	WORD repeat_count() const        { return LOWORD(this->lParam); }
	BYTE scan_code() const           { return LOBYTE(HIWORD(this->lParam)); }
	bool is_extended_key() const     { return (this->lParam >> 24) & 1; }
	bool has_alt_key() const         { return (this->lParam >> 29) & 1; }
	bool key_previously_down() const { return (this->lParam >> 30) & 1; }
	bool key_being_released() const  { return (this->lParam >> 31) & 1; }
PARMDEC(syscolorchange)
PARMDEC(syscommand)
	WORD  command_type() const { return static_cast<WORD>(this->wParam); }
	POINT pos() const          { return { GET_X_LPARAM(this->lParam), GET_Y_LPARAM(this->lParam) }; }
PARMDEC(sysdeadchar)
	WORD char_code() const           { return static_cast<WORD>(this->wParam); }
	WORD repeat_count() const        { return LOWORD(this->lParam); }
	BYTE scan_code() const           { return LOBYTE(HIWORD(this->lParam)); }
	bool is_extended_key() const     { return (this->lParam >> 24) & 1; }
	bool has_alt_key() const         { return (this->lParam >> 29) & 1; }
	bool key_previously_down() const { return (this->lParam >> 30) & 1; }
	bool key_being_released() const  { return (this->lParam >> 31) & 1; }
PARMDEC(syskeydown)
	BYTE vkey_code() const           { return static_cast<BYTE>(this->wParam); }
	WORD repeat_count() const        { return LOWORD(this->lParam); }
	BYTE scan_code() const           { return LOBYTE(HIWORD(this->lParam)); }
	bool is_extended_key() const     { return (this->lParam >> 24) & 1; }
	bool has_alt_key() const         { return (this->lParam >> 29) & 1; }
	bool key_previously_down() const { return (this->lParam >> 30) & 1; }
PARMDEC(syskeyup)
	BYTE vkey_code() const    { return static_cast<BYTE>(this->wParam); }
	WORD repeat_count() const { return LOWORD(this->lParam); }
	BYTE scan_code() const    { return LOBYTE(HIWORD(this->lParam)); }
	bool has_alt_key() const  { return (this->lParam >> 29) & 1; }
PARMDEC(tcard)
	UINT action_id() const   { return static_cast<UINT>(this->wParam); }
	long action_data() const { return static_cast<long>(this->lParam); }
PARMDEC(timechange)
	PARMDEC(timer)
	UINT_PTR  timer_id() const { return static_cast<UINT_PTR>(this->wParam); }
	TIMERPROC callback() const { return reinterpret_cast<TIMERPROC>(this->lParam); }
PARMDEC(uninitmenupopup)
	HMENU hmenu() const   { return reinterpret_cast<HMENU>(this->wParam); }
	WORD  menu_id() const { return HIWORD(this->lParam); }
PARMDEC(userchanged)
	PARMDEC(vkeytoitem)
	BYTE vkey_code() const         { return static_cast<BYTE>(LOWORD(this->wParam)); }
	WORD current_caret_pos() const { return HIWORD(this->wParam); }
	HWND hlistbox() const          { return reinterpret_cast<HWND>(this->lParam); }
PARMDEC(windowposchanged)
	WINDOWPOS& windowpos() const { return *reinterpret_cast<WINDOWPOS*>(this->lParam); }
PARMDEC(windowposchanging)
	WINDOWPOS& windowpos() const { return *reinterpret_cast<WINDOWPOS*>(this->lParam); }
PARMDEC_END

}//namespace wl