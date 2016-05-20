
#pragma once
#include "dlg_dn.h"
#include "chromium_rel.h"
#include "../winutil/internet.h"

// Downloads the list of markers.
class dlg_dn_list final : public dlg_dn {
private:
	winutil::internet_session& _session;
	chromium_rel&              _clist;
	int                        _totBytes;
public:
	dlg_dn_list(winutil::taskbar_progress& taskBar,
		winutil::internet_session& session,
		chromium_rel& clist);
	int get_total_bytes() const;
private:
	bool _download_list(const std::wstring& marker);
	bool _read_xml(const std::vector<BYTE>& buf);
};