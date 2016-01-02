
#pragma once
#include "FrmDn.h"

// Downloads DLL version for a marker.
class FrmDnDll final : public FrmDn {
private:
	wolf::InternetSession& _session;
	std::wstring           _marker;
	int                    _totDownloaded;
public:
	std::wstring version;
	FrmDnDll(wolf::TaskBarProgress& taskBar,
		wolf::InternetSession& session,
		const std::wstring& marker);
private:
	bool _doDownload();
	bool _doReadVersion(std::wstring zipPath);
	static int _findInBinary(const BYTE *pData, size_t dataLen, const wchar_t *what, bool asWideChar);
};