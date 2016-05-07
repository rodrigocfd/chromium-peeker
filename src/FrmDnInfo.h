
#pragma once
#include "FrmDn.h"
#include "../winutil/internet.h"

// Downloads information for a marker.
class FrmDnInfo final : public FrmDn {
public:
	struct Data final {
		std::wstring releaseDate;
		int packageSize;
	};
	
private:
	winutil::internet_session&       _session;
	const std::vector<std::wstring>& _markers;
	int                              _totDownloaded;
public:
	std::vector<Data> data;
	FrmDnInfo(winutil::taskbar_progress& taskBar,
		winutil::internet_session& session,
		const std::vector<std::wstring>& markers);
private:
	bool _doGetOneFile(const std::wstring& marker);
	bool _doProcessFile(const std::vector<BYTE>& buf);
};