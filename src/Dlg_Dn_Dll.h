
#pragma once
#include <winlamb/download.h>
#include "Dlg_Dn.h"

// Downloads DLL version for a marker.
class Dlg_Dn_Dll final : public Dlg_Dn {
private:
	wl::download::session& m_session;
	wstring                m_marker;

public:
	wstring versionNo;

	Dlg_Dn_Dll(wl::progress_taskbar& tb, wl::download::session& sess,
		const wstring& mk);

private:
	void _download();
	void _read_version(wstring zipPath);

	static int _find_in_binary(const BYTE* pData, size_t dataLen,
		const wchar_t* what, bool asWideChar);
};