
#pragma once
#include "Dlg_Dn.h"
#include "Chromium_Rel.h"
#include "../winlamb/download.h"

// Downloads the list of markers.
class Dlg_Dn_List final : public Dlg_Dn {
private:
	wl::download::session& _session;
	Chromium_Rel&          _clist;
	int                    _totBytes;

public:
	Dlg_Dn_List(wl::progress_taskbar& taskBar,
		wl::download::session& session,
		Chromium_Rel& clist);

	int get_total_bytes() const;

private:
	bool _download_list(const std::wstring& marker);
	bool _read_xml(const std::vector<BYTE>& buf);
};