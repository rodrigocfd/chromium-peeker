
#pragma once
#include "DlgDn.h"

// Downloads information for a marker.
class DlgDnInfo final : public DlgDn {
public:
	struct Data final {
		std::wstring releaseDate;
		int packageSize;
	};
	
public:
	std::vector<Data> data;
	DlgDnInfo(wolf::net::Session& session, const std::vector<std::wstring>& markers)
		: _session(session), _markers(markers), _totDownloaded(0) { }
private:
	void events() override;
	bool _doGetOneFile(const std::wstring& marker);
	bool _doProcessFile(const std::vector<BYTE>& buf);

	wolf::net::Session& _session;
	const std::vector<std::wstring>& _markers;
	int _totDownloaded;
};