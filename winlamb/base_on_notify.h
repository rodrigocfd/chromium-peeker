/**
 * Part of WinLamb - Win32 API Lambda Library
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/winlamb
 */

#pragma once
#include "base_inventory.h"
#include "params_notif.h"

namespace wl {

class base_on_notify final {
private:

#define NFYDEC(prefix, ntype, ptype) \
	void ntype(UINT_PTR idFrom, std::function<LONG_PTR(notif::ptype)> func) { \
		this->_inventory.add_notify(idFrom, prefix##ntype, std::move(func)); \
	}

	class _datetimepicker final {
	private:
		base_inventory& _inventory;
	public:
		_datetimepicker(base_inventory& inventory) : _inventory(inventory) { }

#define DTP_DPDEC(ntype, ptype) NFYDEC(DTN_, ntype, ptype)
		DTP_DPDEC(CLOSEUP, hdr)
		DTP_DPDEC(DATETIMECHANGE, datetimechange)
		DTP_DPDEC(DROPDOWN, hdr)
		DTP_DPDEC(FORMAT, datetimeformat)
		DTP_DPDEC(FORMATQUERY, datetimeformatquery)
		DTP_DPDEC(USERSTRING, datetimestring)
		DTP_DPDEC(WMKEYDOWN, datetimewmkeydown)
#define DTP_NMDEC(ntype, ptype) NFYDEC(NM_, ntype, ptype)
		DTP_NMDEC(KILLFOCUS, hdr)
		DTP_NMDEC(SETFOCUS, hdr)
	};

	class _listview final {
	private:
		base_inventory& _inventory;
	public:
		_listview(base_inventory& inventory) : _inventory(inventory) { }

#define LV_LVDEC(ntype, ptype) NFYDEC(LVN_, ntype, ptype)
		LV_LVDEC(BEGINDRAG, listview)
		LV_LVDEC(BEGINLABELEDIT, lvdispinfo)
		LV_LVDEC(BEGINRDRAG, listview)
		LV_LVDEC(BEGINSCROLL, lvscroll)
		LV_LVDEC(COLUMNCLICK, listview)
		LV_LVDEC(COLUMNDROPDOWN, listview)
		LV_LVDEC(COLUMNOVERFLOWCLICK, listview)
		LV_LVDEC(DELETEALLITEMS, listview)
		LV_LVDEC(DELETEITEM, listview)
		LV_LVDEC(ENDLABELEDIT, lvdispinfo)
		LV_LVDEC(ENDSCROLL, lvscroll)
		LV_LVDEC(GETDISPINFO, lvdispinfo)
		LV_LVDEC(GETEMPTYMARKUP, lvemptymarkup)
		LV_LVDEC(GETINFOTIP, lvgetinfotip)
		LV_LVDEC(HOTTRACK, listview)
		LV_LVDEC(INCREMENTALSEARCH, lvfinditem)
		LV_LVDEC(INSERTITEM, listview)
		LV_LVDEC(ITEMACTIVATE, itemactivate)
		LV_LVDEC(ITEMCHANGED, listview)
		LV_LVDEC(ITEMCHANGING, listview)
		LV_LVDEC(KEYDOWN, lvkeydown)
		LV_LVDEC(LINKCLICK, lvlink)
		LV_LVDEC(MARQUEEBEGIN, hdr)
		LV_LVDEC(ODCACHEHINT, lvcachehint)
		LV_LVDEC(ODFINDITEM, lvfinditem)
		LV_LVDEC(ODSTATECHANGED, lvodstatechange)
		LV_LVDEC(SETDISPINFO, lvdispinfo)
#define LV_NMDEC(ntype, ptype) NFYDEC(NM_, ntype, ptype)
		LV_NMDEC(CLICK, itemactivate)
		LV_NMDEC(CUSTOMDRAW, lvcustomdraw)
		LV_NMDEC(DBLCLK, itemactivate)
		LV_NMDEC(HOVER, hdr)
		LV_NMDEC(KILLFOCUS, hdr)
		LV_NMDEC(RCLICK, itemactivate)
		LV_NMDEC(RDBLCLK, itemactivate)
		LV_NMDEC(RELEASEDCAPTURE, hdr)
		LV_NMDEC(RETURN, hdr)
		LV_NMDEC(SETFOCUS, hdr)
	};

	class _monthcalendar final {
	private:
		base_inventory& _inventory;
	public:
		_monthcalendar(base_inventory& inventory) : _inventory(inventory) { }

#define MCA_MCDEC(ntype, ptype) NFYDEC(MCN_, ntype, ptype)
		MCA_MCDEC(GETDAYSTATE, mcdaystate)
		MCA_MCDEC(SELCHANGE, mcselchange)
		MCA_MCDEC(SELECT, mcselchange)
		MCA_MCDEC(VIEWCHANGE, mcviewchange)
#define MCA_NMDEC(ntype, ptype) NFYDEC(NM_, ntype, ptype)
		MCA_NMDEC(RELEASEDCAPTURE, hdr)
	};

	class _treeview final {
	private:
		base_inventory& _inventory;
	public:
		_treeview(base_inventory& inventory) : _inventory(inventory) { }

#define TV_TVDEC(ntype, ptype) NFYDEC(TVN_, ntype, ptype)
		TV_TVDEC(ASYNCDRAW, tvasyncdraw)
		TV_TVDEC(BEGINDRAG, treeview)
		TV_TVDEC(BEGINLABELEDIT, tvdispinfo)
		TV_TVDEC(BEGINRDRAG, treeview)
		TV_TVDEC(DELETEITEM, treeview)
		TV_TVDEC(ENDLABELEDIT, tvdispinfo)
		TV_TVDEC(GETDISPINFO, tvdispinfo)
		TV_TVDEC(GETINFOTIP, tvgetinfotip)
		TV_TVDEC(ITEMCHANGED, tvitemchange)
		TV_TVDEC(ITEMCHANGING, tvitemchange)
		TV_TVDEC(ITEMEXPANDED, treeview)
		TV_TVDEC(ITEMEXPANDING, treeview)
		TV_TVDEC(KEYDOWN, tvkeydown)
		TV_TVDEC(SELCHANGED, treeview)
		TV_TVDEC(SELCHANGING, treeview)
		TV_TVDEC(SETDISPINFO, tvdispinfo)
		TV_TVDEC(SINGLEEXPAND, treeview)
#define TV_NMDEC(ntype, ptype) NFYDEC(NM_, ntype, ptype)
		TV_NMDEC(CLICK, hdr)
		TV_NMDEC(CUSTOMDRAW, tvcustomdraw)
		TV_NMDEC(DBLCLK, hdr)
		TV_NMDEC(KILLFOCUS, hdr)
		TV_NMDEC(RCLICK, hdr)
		TV_NMDEC(RDBLCLK, hdr)
		TV_NMDEC(RETURN, hdr)
		TV_NMDEC(SETCURSOR, mouse)
		TV_NMDEC(SETFOCUS, hdr)
	};

	class _statusbar final {
	private:
		base_inventory& _inventory;
	public:
		_statusbar(base_inventory& inventory) : _inventory(inventory) { }

#define SB_SBDEC(ntype, ptype) NFYDEC(SBN_, ntype, ptype)
		SB_SBDEC(SIMPLEMODECHANGE, hdr)
#define SB_NMDEC(ntype, ptype) NFYDEC(NM_, ntype, ptype)
		MCA_NMDEC(CLICK, mouse)
		MCA_NMDEC(DBLCLK, mouse)
		MCA_NMDEC(RCLICK, mouse)
		MCA_NMDEC(RDBLCLK, mouse)
	};


private:
	base_inventory& _inventory;
public:
	_datetimepicker datetimepicker;
	_listview listview;
	_treeview treeview;

	base_on_notify(base_inventory& inventory) :
		_inventory(inventory), datetimepicker(inventory), listview(inventory), treeview(inventory) { }

	void operator()(UINT_PTR idFrom, UINT code, std::function<LONG_PTR(params::notify)> func) {
		this->_inventory.add_notify(idFrom, code, std::move(func));
	}

	void operator()(std::pair<UINT_PTR, UINT> id, std::function<LONG_PTR(params::notify)> func) {
		this->_inventory.add_notify(id, std::move(func));
	}

	void operator()(std::initializer_list<std::pair<UINT_PTR, UINT>> ids, std::function<LONG_PTR(params::notify)> func) {
		this->_inventory.add_notify(ids, std::move(func));
	}
};

}//namespace wl