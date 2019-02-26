
#pragma once
#include <winlamb/file.h>
#include <winlamb/download.h>
#include "Dlg_Download.h"

// Download the marker zip file to disk, destination chosen by user.
class Dlg_Download_Zip final : public Dlg_Download {
private:
	wl::download::session& m_session;
	std::wstring           m_marker, m_dest;

public:
	Dlg_Download_Zip(wl::progress_taskbar& tb, wl::download::session& sess,
		const std::wstring& mk);

private:
	void _download();
};