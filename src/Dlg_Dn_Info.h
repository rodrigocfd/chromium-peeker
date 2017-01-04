
#pragma once
#include "Dlg_Dn.h"
#include "../winlamb/download.h"

// Downloads information for a marker.
class Dlg_Dn_Info final : public Dlg_Dn {
public:
	struct Data final {
		std::wstring releaseDate;
		int          packageSize;
	};
	
private:
	wl::download::session&           _session;
	const std::vector<std::wstring>& _markers;
	int                              _totDownloaded;

public:
	std::vector<Data> data;

	Dlg_Dn_Info(wl::progress_taskbar& taskBar,
		wl::download::session& session,
		const std::vector<std::wstring>& markers);

private:
	bool _get_one_file(const std::wstring& marker);
	bool _process_file(const std::vector<BYTE>& buf);
};