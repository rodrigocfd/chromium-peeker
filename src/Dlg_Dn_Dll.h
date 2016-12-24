
#pragma once
#include "Dlg_Dn.h"
#include "../wet/download.h"

// Downloads DLL version for a marker.
class Dlg_Dn_Dll final : public Dlg_Dn {
private:
	wet::download::session& _session;
	std::wstring            _marker;
	int                     _totDownloaded;

public:
	std::wstring version;

	Dlg_Dn_Dll(wet::progress_taskbar& taskBar,
		wet::download::session& session,
		const std::wstring& marker);

private:
	INT_PTR proc(wet::params p) override;
	bool   _download();
	bool   _read_version(std::wstring zipPath);

	static int _find_in_binary(const BYTE* pData, size_t dataLen,
		const wchar_t* what, bool asWideChar);
};