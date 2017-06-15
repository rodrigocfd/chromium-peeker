
#pragma once
#include "Dlg_Dn.h"
#include "../winlamb-more/download.h"

// Downloads information for a marker.
class Dlg_Dn_Info final : public Dlg_Dn {
public:
	struct Data final {
		std::wstring releaseDate;
		int          packageSize;
	};
	
private:
	wl::download::session&           m_session;
	const std::vector<std::wstring>& m_markers;
	int                              m_totDownloaded;

public:
	std::vector<Data> data;

	Dlg_Dn_Info(wl::progress_taskbar& tb, wl::download::session& sess,
		const std::vector<std::wstring>& mk);

private:
	bool _get_one_file(const std::wstring& marker);
	bool _process_file(const std::vector<BYTE>& blob);
};