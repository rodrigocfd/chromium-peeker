
#pragma once
#include "Dlg_Dn.h"
#include "../wet/download.h"

// Downloads information for a marker.
class Dlg_Dn_Info final : public Dlg_Dn {
public:
	struct Data final {
		std::wstring releaseDate;
		int          packageSize;
	};
	
private:
	wet::download::session&          _session;
	const std::vector<std::wstring>& _markers;
	int                              _totDownloaded;

public:
	std::vector<Data> data;

	Dlg_Dn_Info(wet::progress_taskbar& taskBar,
		wet::download::session& session,
		const std::vector<std::wstring>& markers);

private:
	INT_PTR proc(wet::params p) override;
	bool   _get_one_file(const std::wstring& marker);
	bool   _process_file(const std::vector<BYTE>& buf);
};