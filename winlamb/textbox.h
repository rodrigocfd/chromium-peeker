/**
 * Part of WinLamb - Win32 API Lambda Library
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/winlamb
 */

#pragma once
#include "base_native_control.h"
#include "base_subclass.h"
#include "plus_control.h"
#include "plus_text.h"

namespace wl {

class textbox final : public plus_control<textbox>, public plus_text<textbox> {
public:
	struct selection final {
		int start;
		int len;
	};

private:
	base_native_control _control;
	base_subclass _subclass;
	std::function<void(BYTE)> _onKeyUp;

public:
	textbox() : plus_control(*this), plus_text(*this) {
		this->_subclass.on.KEYDOWN([&](params::keydown p)->LRESULT {
			if (p.vkey_code() == VK_ESCAPE) { // ESC http://www.williamwilling.com/blog/?p=28
				SendMessageW(GetAncestor(this->hwnd(), GA_PARENT), WM_COMMAND,
					IDCANCEL, reinterpret_cast<LPARAM>(this->hwnd()));
				return 0;
			}
			return DefSubclassProc(this->hwnd(), p.message, p.wParam, p.lParam);
		});
		this->_subclass.on.GETDLGCODE([&](params::getdlgcode p)->LRESULT {
			bool hasCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
			if (p.lParam && p.vkey_code() == 'A' && hasCtrl) { // Ctrl+A to select all text
				p.wParam = 0; // prevent propagation, therefore beep
				SendMessageW(this->hwnd(), EM_SETSEL, 0, -1);
				return DLGC_WANTCHARS;
			}
			return DefSubclassProc(this->hwnd(), p.message, p.wParam, p.lParam);
		});
		this->_subclass.on.KEYUP([&](params::keyup p)->LRESULT {
			if (this->_onKeyUp) {
				this->_onKeyUp(p.vkey_code());
				return 0;
			}
			return DefSubclassProc(this->hwnd(), p.message, p.wParam, p.lParam);
		});
	}

	HWND     hwnd() const                    { return this->_control.hwnd(); }
	textbox& be(HWND hWnd)                   { this->_control.be(hWnd); return this->_subc(); }
	textbox& be(HWND hParent, int controlId) { this->_control.be(hParent, controlId); return this->_subc(); }

	textbox& create(HWND hParent, int controlId, POINT pos, LONG width) {
		return this->_raw_create(hParent, controlId, pos, {width,21}, ES_AUTOHSCROLL);
	}

	textbox& create_password(HWND hParent, int id, POINT pos, LONG width) {
		return this->_raw_create(hParent, id, pos, {width,21}, ES_AUTOHSCROLL | ES_PASSWORD);
	}

	textbox& create_multi_line(HWND hParent, int controlId, POINT pos, SIZE size) {
		return this->_raw_create(hParent, controlId, pos, size, ES_MULTILINE | ES_WANTRETURN);
	}

	textbox& textbox::selection_set(selection selec) {
		SendMessageW(this->hwnd(), EM_SETSEL, selec.start, selec.start + selec.len);
		return *this;
	}

	textbox& selection_set_all() {
		return this->selection_set({0, -1});
	}

	selection selection_get() const {
		int p0 = 0, p1 = 0;
		SendMessageW(this->hwnd(), EM_GETSEL,
			reinterpret_cast<WPARAM>(&p0), reinterpret_cast<LPARAM>(&p1));
		return { p0, p1 - p0 }; // start, length
	}

	textbox& selection_replace(const wchar_t* t) {
		SendMessageW(this->hwnd(), EM_REPLACESEL,
			TRUE, reinterpret_cast<LPARAM>(t));
		return *this;
	}

	textbox& selection_replace(const std::wstring& t) {
		return this->selection_replace(t.c_str());
	}

	textbox& on_key_up(std::function<void(BYTE)> func) {
		this->_onKeyUp = std::move(func);
		return *this;
	}

private:
	textbox& _raw_create(HWND hParent, int controlId, POINT pos, SIZE size, DWORD extraStyles) {
		this->_control.create(hParent, controlId, nullptr,
			pos, size, L"Edit",
			WS_CHILD | WS_VISIBLE | extraStyles,
			WS_EX_CLIENTEDGE);
		return this->_subc();
	}

	textbox& _subc() {
		this->_subclass.install_subclass(this->hwnd());
		return *this;
	}
};

}//namespace wl