
#pragma once
#include "FrmDn.h"
#include "ChromiumRel.h"

// Downloads the list of markers.
class FrmDnList final : public FrmDn {
private:
	wolf::InternetSession& _session;
	ChromiumRel& _clist;
	int _totBytes;
public:
	FrmDnList(wolf::WindowMain *wmain,
		wolf::InternetSession& session,
		ChromiumRel& clist);
	int getTotalBytes() const;
private:
	bool _doDownloadList(const std::wstring& marker);
	bool _doReadXml(const std::vector<BYTE>& buf);
};