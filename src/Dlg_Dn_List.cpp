
#include "Dlg_Dn_List.h"
#include <winlamb/str.h>
#include <winlamb/thread.h>
#include <winlamb/xml.h>
using namespace wl;

Dlg_Dn_List::Dlg_Dn_List(progress_taskbar& tb, download::session& sess,
	Chromium_Rel& clst)
	: Dlg_Dn(tb), m_session(sess), m_clist(clst), m_totBytes(0)
{
	on_message(WM_INITDIALOG, [&](wm::initdialog)
	{
		init_controls();
		set_text(L"No markers downloaded...");
		m_progBar.set_waiting(true);

		thread::run_detached([&]() {
			_download_list(L""); // start downloading first batch of markers
		});

		return TRUE;
	});

	handle_close_msg();
}

void Dlg_Dn_List::_download_list(const wstring& marker)
{
	wstring lnk = str::format(L"%s/?delimiter=/&prefix=Win/", BASE_URL);
	if (!marker.empty()) {
		lnk.append(L"&marker=").append(marker);
	}

	download dl(m_session, lnk);
	dl.set_referrer(REFERRER);
	dl.add_request_header(L"Accept-Encoding", L"gzip,deflate,sdch")
		.add_request_header(L"Connection", L"keep-alive")
		.add_request_header(L"DNT", L"1")
		.add_request_header(L"Host", L"commondatastorage.googleapis.com");

	dl.on_start([&]() {
		run_ui_thread([&]() {
			m_progBar.set_waiting(false).set_pos(0);
			m_taskbarProg.set_waiting(true);
			m_lblTitle.set_text(L"XML download started...");
		});
	});

	dl.on_progress([&]() {
		run_ui_thread([&]() {
			m_progBar.set_pos(dl.get_percent());
			m_lblTitle.set_text( str::format(L"%.2f%% downloaded (%.2f KB)...\n",
				dl.get_percent(),
				static_cast<float>(dl.get_total_downloaded()) / 1024) );
		});
	});

	try {
		dl.start();
	} catch (const std::exception& e) {
		show_err_and_close(L"Download error", str::parse_ascii(e.what()));
		return;
	}

	_read_xml(dl.data);
}

void Dlg_Dn_List::_read_xml(const vector<BYTE>& blob)
{
	wstring xmlStr;
	try {
		xmlStr = str::parse_blob(blob);
	} catch (const std::exception& e) {
		show_err_and_close(L"XML parsing error", str::parse_ascii(e.what()));
		return;
	}

	xml xmlc = xmlStr;
	m_clist.append(xmlc);
	m_totBytes += blob.size();

	run_ui_thread([&]() {
		set_text(str::format(L"%d markers downloaded (%.2f KB)...",
			m_clist.markers().size(),
			static_cast<float>(m_totBytes) / 1024) );
	});
	
	if (!m_clist.is_finished()) {
		run_ui_thread([&]() {
			m_lblTitle.set_text( str::format(L"Next marker: %s...\n", m_clist.next_marker()) );
		});
		_download_list(m_clist.next_marker()); // proceed to next marker
	} else {
		run_ui_thread([&]() {
			m_taskbarProg.clear();
			EndDialog(hwnd(), IDOK); // all markers downloaded
		});
	}
}