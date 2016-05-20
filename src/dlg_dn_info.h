
#pragma once
#include "dlg_dn.h"
#include "../winutil/internet.h"

// Downloads information for a marker.
class dlg_dn_info final : public dlg_dn {
public:
	struct Data final {
		std::wstring releaseDate;
		int          packageSize;
	};
	
private:
	winutil::internet_session&       _session;
	const std::vector<std::wstring>& _markers;
	int                              _totDownloaded;
public:
	std::vector<Data> data;
	dlg_dn_info(winutil::taskbar_progress& taskBar,
		winutil::internet_session& session,
		const std::vector<std::wstring>& markers);
private:
	bool _get_one_file(const std::wstring& marker);
	bool _process_file(const std::vector<BYTE>& buf);
};