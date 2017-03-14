/**
 * Part of WinLamb - Win32 API Lambda Library
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/winlamb
 */

#pragma once
#include "base_dialog.h"
#include "base_text.h"

/**
 *                              +--------------------- msgs_[any] <-------------------------+
 *             +-- base_msgs <--+                                                           |
 *             |                +-- base_threaded <--+                                      +-- [user]
 * base_wnd <--+                                     +-- base_dialog <--+                   |
 *             +------------ base_wheel <------------+                  +-- dialog_modal <--+
 *             |                                                        |
 *             +--------------------- base_text <-----------------------+
 */

namespace wl {

// Inherit from this class to have a modal dialog popup.
class dialog_modal :
	public    base::dialog,
	protected base::text<dialog_modal>
{
protected:
	base::dialog::setup_vars setup;

	dialog_modal(size_t msgsReserve = 0) : dialog(msgsReserve + 1) {
		this->on_message(WM_CLOSE, [&](const params&)->INT_PTR {
			EndDialog(this->hwnd(), IDOK);
			return TRUE;
		});
	}

public:
	int show(const base::wnd* parent) {
		if (!this->dialog::_basic_initial_checks(this->setup)) return -1;
		return static_cast<int>(DialogBoxParamW(
			reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(parent->hwnd(), GWLP_HINSTANCE)),
			MAKEINTRESOURCE(this->setup.dialogId), parent->hwnd(), base::dialog::_dialog_proc,
			reinterpret_cast<LPARAM>(this)) );
	}

protected:
	void center_on_parent() const {
		RECT rc = { 0 }, rcParent = { 0 };
		GetWindowRect(this->hwnd(), &rc);
		GetWindowRect(GetParent(this->hwnd()), &rcParent); // both relative to screen
		SetWindowPos(this->hwnd(), nullptr,
			rcParent.left + (rcParent.right - rcParent.left)/2 - (rc.right - rc.left)/2,
			rcParent.top + (rcParent.bottom - rcParent.top)/2 - (rc.bottom - rc.top)/2,
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
};

}//namespace wl