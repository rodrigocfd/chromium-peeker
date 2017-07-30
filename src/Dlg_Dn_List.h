
#pragma once
#include <winlamb-more/download.h>
#include "Dlg_Dn.h"
#include "Chromium_Rel.h"

// Downloads the list of markers.
class Dlg_Dn_List final : public Dlg_Dn {
private:
	wl::download::session& m_session;
	Chromium_Rel&          m_clist;
	size_t                 m_totBytes;

public:
	Dlg_Dn_List(wl::progress_taskbar& tb, wl::download::session& sess,
		Chromium_Rel& clst);

	size_t get_total_bytes() const;

private:
	bool _download_list(const std::wstring& marker);
	bool _read_xml(const std::vector<BYTE>& blob);
};