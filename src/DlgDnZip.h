
#pragma once
#include "DlgDn.h"

// Download the marker zip file to disk, destination chosen by user.
class DlgDnZip final : public DlgDn {
public:
	DlgDnZip(wolf::net::Session& session, const std::wstring& marker)
		: _session(session), _marker(marker) { }
private:
	void events() override;
	bool _doDownload();
	bool _doReceiveData(wolf::net::Download& zipdl, wolf::file::Raw& fout);

	wolf::net::Session& _session;
	std::wstring        _marker, _dest;
};