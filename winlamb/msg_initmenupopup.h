/**
 * Part of WinLamb - Windows Lambda Library
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/winlamb
 */

#pragma once
#include "wnd_proc.h"
#include "traits_window.h"
#include "traits_dialog.h"

/**
 * msg_initmenupopup
 *  wnd_proc
 *   wnd
 */

namespace winlamb {

template<typename traitsT>
class msg_initmenupopup : virtual public wnd_proc<traitsT> {
public:
	struct params_initmenupopup : public params {
		params_initmenupopup(const params& p) { wParam = p.wParam; lParam = p.lParam; }
		HMENU hmenu() const                   { return reinterpret_cast<HMENU>(wParam); }
		WORD item_rel_pos() const             { return LOWORD(lParam); }
	};
	typedef std::function<typename traitsT::ret_type(params_initmenupopup)> func_initmenupopup_type;

private:
	callback_depot<WORD, func_initmenupopup_type, params_initmenupopup, traitsT> _callbacks;

protected:
	msg_initmenupopup()
	{
		on_message(WM_INITMENUPOPUP, [this](params p)->typename traitsT::ret_type {
			params_initmenupopup pi(p);
			return _callbacks.process(hwnd(), WM_INITMENUPOPUP,
				GetMenuItemID(pi.hmenu(), 0), pi);
		});
	}

public:
	virtual ~msg_initmenupopup() = default;

	void on_initmenupopup(WORD commandIdOfFirstItem, func_initmenupopup_type callback)
	{
		_callbacks.add(commandIdOfFirstItem, std::move(callback));
	}

	void on_initmenupopup(std::initializer_list<WORD> commandIdOfFirstItems, func_initmenupopup_type callback)
	{
		_callbacks.add(commandIdOfFirstItems, std::move(callback));
	}
};

typedef msg_initmenupopup<traits_window> window_msg_initmenupopup;
typedef msg_initmenupopup<traits_dialog> dialog_msg_initmenupopup;

}//namespace winlamb