
#pragma once
#include "DlgDn.h"

// Downloads DLL version for a marker.
class DlgDnDll final : public DlgDn {
public:
	std::wstring version;
	DlgDnDll(wolf::net::Session& session, const std::wstring& marker)
		: _session(session), _marker(marker), _totDownloaded(0) { }
private:
	void events() override;
	bool _doDownload();
	bool _doReadVersion(std::wstring zipPath);

	wolf::net::Session& _session;
	std::wstring _marker;
	int _totDownloaded;
};