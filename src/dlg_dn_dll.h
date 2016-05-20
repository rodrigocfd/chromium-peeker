
#pragma once
#include "dlg_dn.h"
#include "../winutil/internet.h"

// Downloads DLL version for a marker.
class dlg_dn_dll final : public dlg_dn {
private:
	winutil::internet_session& _session;
	std::wstring               _marker;
	int                        _totDownloaded;
public:
	std::wstring version;
	dlg_dn_dll(winutil::taskbar_progress& taskBar,
		winutil::internet_session& session,
		const std::wstring& marker);
private:
	bool _download();
	bool _read_version(std::wstring zipPath);
	static int _find_in_binary(const BYTE* pData, size_t dataLen, const wchar_t* what, bool asWideChar);
};