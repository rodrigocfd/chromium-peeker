
#pragma once
#include "params.h"

namespace wl {

struct notif final {
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
	struct hdr;
	struct itemactivate;
	struct mouse;

	struct datetimechange;
	struct datetimeformat;
	struct datetimeformatquery;
	struct datetimestring;
	struct datetimewmkeydown;

	struct listview;
	struct lvdispinfo;
	struct lvcachehint;
	struct lvcustomdraw;
	struct lvemptymarkup;
	struct lvfinditem;
	struct lvgetinfotip;
	struct lvkeydown;
	struct lvlink;
	struct lvodstatechange;
	struct lvscroll;

	struct mcdaystate;
	struct mcselchange;
	struct mcviewchange;

	struct treeview;
	struct tvasyncdraw;
	struct tvcustomdraw;
	struct tvdispinfo;
	struct tvgetinfotip;
	struct tvitemchange;
	struct tvkeydown;
};


#define PARMDECN(sname, ptype) \
	struct notif::sname final : public notif::_break_compat { \
		ptype& data; \
		sname(const params& p) : \
			_break_compat(p), data(*reinterpret_cast<ptype*>(p.lParam)) { } \
	};

PARMDECN(hdr, NMHDR)
PARMDECN(itemactivate, NMITEMACTIVATE)
PARMDECN(mouse, NMMOUSE)

PARMDECN(datetimechange, NMDATETIMECHANGE)
PARMDECN(datetimeformat, NMDATETIMEFORMAT)
PARMDECN(datetimeformatquery, NMDATETIMEFORMATQUERY)
PARMDECN(datetimestring, NMDATETIMESTRING)
PARMDECN(datetimewmkeydown, NMDATETIMEWMKEYDOWN)

PARMDECN(listview, NMLISTVIEW)
PARMDECN(lvcachehint, NMLVCACHEHINT)
PARMDECN(lvcustomdraw, NMLVCUSTOMDRAW)
PARMDECN(lvdispinfo, NMLVDISPINFO)
PARMDECN(lvemptymarkup, NMLVEMPTYMARKUP)
PARMDECN(lvfinditem, NMLVFINDITEM)
PARMDECN(lvgetinfotip, NMLVGETINFOTIP)
PARMDECN(lvkeydown, NMLVKEYDOWN)
PARMDECN(lvlink, NMLVLINK)
PARMDECN(lvodstatechange, NMLVODSTATECHANGE)
PARMDECN(lvscroll, NMLVSCROLL)

PARMDECN(mcdaystate, NMDAYSTATE)
PARMDECN(mcselchange, NMSELCHANGE)
PARMDECN(mcviewchange, NMVIEWCHANGE)

PARMDECN(treeview, NMTREEVIEW)
PARMDECN(tvasyncdraw, NMTVASYNCDRAW)
PARMDECN(tvcustomdraw, NMTVCUSTOMDRAW)
PARMDECN(tvdispinfo, NMTVDISPINFO)
PARMDECN(tvgetinfotip, NMTVGETINFOTIP)
PARMDECN(tvitemchange, NMTVITEMCHANGE)
PARMDECN(tvkeydown, NMTVKEYDOWN)

}//namespace wl