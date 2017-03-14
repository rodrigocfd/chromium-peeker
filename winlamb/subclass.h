/**
 * Part of WinLamb - Win32 API Lambda Library
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/winlamb
 */

#pragma once
#include "base_threaded.h"

/**
 *             +---------------- msgs_[any] <-----------------+
 * base_wnd <--+                                              +-- [user]
 *             +-- base_msgs <-- base_threaded <-- subclass --+
 */

namespace wl {

// Manages window subclassing in a window.
class subclass final : public base::threaded {
private:
	UINT _subclassId;
public:
	~subclass() { this->remove_subclass(); }

	explicit subclass(size_t msgsReserve = 0) : threaded(0L, msgsReserve) {
		this->msgs::_defProc = [&](const params& p)->LRESULT { // overwrite default procedure
			return DefSubclassProc(this->hwnd(), p.message, p.wParam, p.lParam);
		};
	}

	void remove_subclass() {
		if (this->hwnd()) {
			RemoveWindowSubclass(this->hwnd(), _proc, this->_subclassId);
			this->wnd::_hWnd = nullptr;
		}
	}

	void install_subclass(HWND hWnd) {
		this->remove_subclass();
		this->wnd::_hWnd = hWnd;
		if (hWnd) {
			this->_subclassId = _next_id();
			SetWindowSubclass(this->hwnd(), _proc, this->_subclassId,
				reinterpret_cast<DWORD_PTR>(this));
		}
	}

private:
	static LRESULT CALLBACK _proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp,
		UINT_PTR idSubclass, DWORD_PTR refData)
	{
		subclass* pSelf = reinterpret_cast<subclass*>(refData);
		if (pSelf && pSelf->hwnd()) {
			funcT* pFunc = pSelf->_msgInventory.find(msg);
			if (pFunc) {
				LRESULT ret = (*pFunc)(params{msg, wp, lp});
				if (msg == WM_NCDESTROY) {
					pSelf->remove_subclass();
				}
				return ret;
			}
		}

		if (msg == WM_NCDESTROY) {
			RemoveWindowSubclass(hWnd, _proc, idSubclass);
		}
		return DefSubclassProc(hWnd, msg, wp, lp);
	}

	static UINT _next_id() {
		static UINT firstId = 0;
		return firstId++;
	}
};

}//namespace wl