
#pragma once
#include "dlg_dn.h"
#include "../winutil/file.h"
#include "../winutil/internet.h"

// Download the marker zip file to disk, destination chosen by user.
class dlg_dn_zip final : public dlg_dn {
private:
	winutil::internet_session& _session;
	std::wstring               _marker, _dest;
public:
	dlg_dn_zip(winutil::taskbar_progress& taskBar,
		winutil::internet_session& session,
		const std::wstring& marker);
private:
	bool _download();
	bool _receive_data(winutil::internet_download& zipdl, winutil::file& fout);
};