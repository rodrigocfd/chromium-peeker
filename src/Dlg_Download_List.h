
#pragma once
#include "../winlamb/download.h"
#include "Dlg_Download.h"
#include "Chromium_Rel.h"

// Downloads the list of markers.
class Dlg_Download_List final : public Dlg_Download {
private:
	wl::download::session& m_session;
	Chromium_Rel&          m_clist;
	size_t                 m_totBytes;

public:
	Dlg_Download_List(wl::progress_taskbar& tb, wl::download::session& sess,
		Chromium_Rel& clst);

	size_t get_total_bytes() const { return m_totBytes; }

private:
	void _download_list(const std::wstring& marker);
	void _read_xml(const std::vector<BYTE>& blob);
};