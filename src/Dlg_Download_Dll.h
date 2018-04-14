
#pragma once
#include <winlamb/download.h>
#include "Dlg_Download.h"

// Downloads DLL version for a marker.
class Dlg_Download_Dll final : public Dlg_Download {
private:
	wl::download::session& m_session;
	wstring                m_marker;

public:
	wstring versionNo;

	Dlg_Download_Dll(wl::progress_taskbar& tb, wl::download::session& sess,
		const wstring& mk);

private:
	void _download();
	void _read_version(wstring zipPath);
};