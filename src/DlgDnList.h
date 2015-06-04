
#pragma once
#include "DlgDn.h"
#include "ChromiumRel.h"

// Downloads the list of markers.
class DlgDnList final : public DlgDn {
public:
	DlgDnList(wolf::net::Session& session, ChromiumRel& clist)
		: _session(session), _clist(clist), _totBytes(0) { }
	int getTotalBytes() const { return _totBytes; }
private:
	void events() override;
	bool _doDownloadList(const std::wstring& marker);
	bool _doReadXml(const std::vector<BYTE>& buf);

	wolf::net::Session& _session;
	ChromiumRel& _clist;
	int _totBytes;
};