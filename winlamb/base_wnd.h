/**
 * Part of WinLamb - Win32 API Lambda Library
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/winlamb
 */

#pragma once
#include <Windows.h>

namespace wl {

class subclass;

namespace base {

	class dialog;
	class window;
	class native_control;

	class wnd {
	private:
		friend subclass;
		friend dialog;
		friend window;
		friend native_control;

		HWND _hWnd; // can be set only by friends
	protected:
		wnd() : _hWnd(nullptr) { }
	public:
		HWND hwnd() const { return this->_hWnd; }
	};

}//namespace base
}//namespace wl