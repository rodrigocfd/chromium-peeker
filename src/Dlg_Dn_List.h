
#pragma once
#include "Dlg_Dn.h"
#include "Chromium_Rel.h"
#include "../wet/download.h"

// Downloads the list of markers.
class Dlg_Dn_List final : public Dlg_Dn {
private:
	wet::download::session& _session;
	Chromium_Rel&           _clist;
	int                     _totBytes;

public:
	Dlg_Dn_List(wet::progress_taskbar& taskBar,
		wet::download::session& session,
		Chromium_Rel& clist);

	int get_total_bytes() const;

private:
	INT_PTR proc(wet::params p) override;
	bool   _download_list(const std::wstring& marker);
	bool   _read_xml(const std::vector<BYTE>& buf);
};