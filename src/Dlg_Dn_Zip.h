
#pragma once
#include "Dlg_Dn.h"
#include "../winlamb/file.h"
#include "../winlamb/download.h"

// Download the marker zip file to disk, destination chosen by user.
class Dlg_Dn_Zip final : public Dlg_Dn {
private:
	wl::download::session& _session;
	std::wstring           _marker, _dest;

public:
	Dlg_Dn_Zip(wl::progress_taskbar& taskBar,
		wl::download::session& session,
		const std::wstring& marker);

private:
	bool _download();
	bool _receive_data(wl::download& zipdl, wl::file& fout);
};