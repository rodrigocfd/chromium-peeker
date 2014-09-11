//
// Automation to aid WM_INITMENUPOPUP handling.
// Part of TOOLOW - Thin Object Oriented Layer Over Win32.
// @author Rodrigo Cesar de Freitas Dias
// @see https://github.com/rodrigocfd/toolow
//

#include <Windows.h>

class InitMenuPopup final {
private:
	HMENU _hMenu;
public:
	explicit InitMenuPopup(WPARAM wp) : _hMenu((HMENU)wp) { }

	bool isFirst(int cmdItemId) {
		return ::GetMenuItemID(_hMenu, 0) == cmdItemId;
	}

	InitMenuPopup& enable(int cmdItemId, bool doEnable) {
		::EnableMenuItem(_hMenu, cmdItemId, MF_BYCOMMAND |
			((doEnable) ? MF_ENABLED : MF_GRAYED));
		return *this;
	}

	InitMenuPopup& setDefault(int cmdItemId) {
		::SetMenuDefaultItem(_hMenu, cmdItemId, MF_BYCOMMAND);
		return *this;
	}
};