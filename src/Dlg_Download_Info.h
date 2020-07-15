
#pragma once
#include "../winlamb/download.h"
#include "Dlg_Download.h"

// Downloads information for a marker.
class Dlg_Download_Info final : public Dlg_Download {
public:
	struct Data final {
		std::wstring releaseDate;
		int          packageSize;
	};

private:
	wl::download::session&           m_session;
	const std::vector<std::wstring>& m_markers;
	int                              m_totDownloaded = 0;

public:
	std::vector<Data> data;

	Dlg_Download_Info(wl::progress_taskbar& tb, wl::download::session& sess,
		const std::vector<std::wstring>& mk);

private:
	void _get_one_file(const std::wstring& marker);
	void _process_file(const std::vector<BYTE>& blob);
};