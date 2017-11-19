
#pragma once
#include <winlamb/file.h>
#include <winlamb/download.h>
#include "Dlg_Dn.h"

// Download the marker zip file to disk, destination chosen by user.
class Dlg_Dn_Zip final : public Dlg_Dn {
private:
	wl::download::session& m_session;
	wstring                m_marker, m_dest;

public:
	Dlg_Dn_Zip(wl::progress_taskbar& tb, wl::download::session& sess, const wstring& mk);

private:
	void _download();
};