
#pragma once
#include "Dlg_Dn.h"
#include "../wet/file.h"
#include "../wet/download.h"

// Download the marker zip file to disk, destination chosen by user.
class Dlg_Dn_Zip final : public Dlg_Dn {
private:
	wet::download::session& _session;
	std::wstring            _marker, _dest;

public:
	Dlg_Dn_Zip(wet::progress_taskbar& taskBar,
		wet::download::session& session,
		const std::wstring& marker);

private:
	INT_PTR proc(wet::params p) override;
	bool   _download();
	bool   _receive_data(wet::download& zipdl, wet::file& fout);
};