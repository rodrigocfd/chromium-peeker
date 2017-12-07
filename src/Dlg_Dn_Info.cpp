
#include "Dlg_Dn_Info.h"
#include <winlamb/str.h>
#include <winlamb/thread.h>
#include <winlamb/xml.h>
using namespace wl;

Dlg_Dn_Info::Dlg_Dn_Info(progress_taskbar& tb, download::session& sess,
	const vector<wstring>& mk)
	: Dlg_Dn(tb), m_session(sess), m_markers(mk)
{
	on_message(WM_INITDIALOG, [&](wm::initdialog)
	{
		init_controls();
		set_text(L"Downloading...");
		m_progBar.set_waiting(true);

		thread::run_detached([&]() {
			_get_one_file(m_markers[0]); // proceed with first file
		});

		return TRUE;
	});

	handle_close_msg();
}

void Dlg_Dn_Info::_get_one_file(const wstring& marker)
{
	wstring lnk = str::format(L"%s/?delimiter=/&prefix=%s", BASE_URL, marker);

	download dl(m_session, lnk);
	dl.set_referrer(REFERRER);
	dl.add_request_header(L"Accept-Encoding", L"gzip,deflate,sdch")
		.add_request_header(L"Connection", L"keep-alive")
		.add_request_header(L"DNT", L"1")
		.add_request_header(L"Host", L"commondatastorage.googleapis.com");

	try {
		dl.start();
	} catch (const std::exception& e) {
		show_err_and_close(L"Download error", str::to_wstring(e.what()));
		return;
	}

	_process_file(dl.data);
}

void Dlg_Dn_Info::_process_file(const vector<BYTE>& blob)
{
	m_totDownloaded += static_cast<int>(blob.size());
	run_ui_thread([&]() {
		m_lblTitle.set_text( str::format(L"%d/%d markers (%.2f KB)...",
			data.size(), m_markers.size(),
			static_cast<float>(m_totDownloaded) / 1024) );
		double pct = (static_cast<float>(data.size()) / m_markers.size()) * 100;
		m_progBar.set_pos(pct);
		m_taskbarProg.set_pos(pct);
	});

	wstring xmlStr;
	try {
		xmlStr = str::to_wstring(blob);
	} catch (const std::exception& e) {
		show_err_and_close(L"XML parsing error", str::to_wstring(e.what()));
		return;
	}

	xml xmlc = xmlStr;
	data.resize(data.size() + 1); // realloc public return buffer

	vector<xml::node*> cnodes = xmlc.root.children_by_name(L"Contents");
	for (xml::node *cnode : cnodes) {
		if (str::ends_withi(cnode->first_child_by_name(L"Key")->value, L"chrome-win32.zip")) {
			data.back().releaseDate = cnode->first_child_by_name(L"LastModified")->value;
			data.back().packageSize = std::stoi(cnode->first_child_by_name(L"Size")->value);
			break;
		}
	}

	if (data.size() == m_markers.size()) {
		run_ui_thread([&]() {
			m_taskbarProg.clear();
			EndDialog(hwnd(), IDOK); // last file has been processed
		});
	} else {
		_get_one_file( m_markers[data.size()] ); // proceed to next file
	}
}