
#pragma once
#include "FrmDn.h"
#include "../winutil/file.h"
#include "../winutil/internet.h"

// Download the marker zip file to disk, destination chosen by user.
class FrmDnZip final : public FrmDn {
private:
	winutil::internet_session& _session;
	std::wstring               _marker, _dest;
public:
	FrmDnZip(winutil::taskbar_progress& taskBar,
		winutil::internet_session& session,
		const std::wstring& marker);
private:
	bool _doDownload();
	bool _doReceiveData(winutil::internet_download& zipdl, winutil::file& fout);
};