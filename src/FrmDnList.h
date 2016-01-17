
#pragma once
#include "FrmDn.h"
#include "ChromiumRel.h"
#include "../winutil/Internet.h"

// Downloads the list of markers.
class FrmDnList final : public FrmDn {
private:
	InternetSession& _session;
	ChromiumRel&     _clist;
	int              _totBytes;
public:
	FrmDnList(TaskBarProgress& taskBar,
		InternetSession& session,
		ChromiumRel& clist);
	int getTotalBytes() const;
private:
	bool _doDownloadList(const std::wstring& marker);
	bool _doReadXml(const std::vector<BYTE>& buf);
};