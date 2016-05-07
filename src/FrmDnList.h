
#pragma once
#include "FrmDn.h"
#include "ChromiumRel.h"
#include "../winutil/internet.h"

// Downloads the list of markers.
class FrmDnList final : public FrmDn {
private:
	winutil::internet_session& _session;
	ChromiumRel&               _clist;
	int                        _totBytes;
public:
	FrmDnList(winutil::taskbar_progress& taskBar,
		winutil::internet_session& session,
		ChromiumRel& clist);
	int getTotalBytes() const;
private:
	bool _doDownloadList(const std::wstring& marker);
	bool _doReadXml(const std::vector<BYTE>& buf);
};