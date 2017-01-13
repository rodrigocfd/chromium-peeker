
#pragma once
#include "Dlg_Dn.h"
#include "Chromium_Rel.h"
#include "../winlamb/download.h"

// Downloads the list of markers.
class Dlg_Dn_List final : public Dlg_Dn {
private:
	wl::download::session& m_session;
	Chromium_Rel&          m_clist;
	int                    m_totBytes;

public:
	Dlg_Dn_List(wl::progress_taskbar& tb, wl::download::session& sess,
		Chromium_Rel& clst);

	int get_total_bytes() const;

private:
	bool _download_list(const std::wstring& marker);
	bool _read_xml(const std::vector<BYTE>& buf);
};