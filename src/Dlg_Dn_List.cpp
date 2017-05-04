
#include "Dlg_Dn_List.h"
#include "../winlamb-more/str.h"
#include "../winlamb-more/sys.h"
#include "../winlamb-more/xml.h"
using namespace wl;
using std::vector;
using std::wstring;

Dlg_Dn_List::Dlg_Dn_List(progress_taskbar& tb, download::session& sess,
	Chromium_Rel& clst)
	: Dlg_Dn(tb), m_session(sess), m_clist(clst), m_totBytes(0)
{
	on_message(WM_INITDIALOG, [&](params&)
	{
		init_controls();
		SetWindowText(hwnd(), L"No markers downloaded...");
		sys::thread([&]() {
			_download_list(L""); // start downloading first batch of markers
		});
		return TRUE;
	});
}

int Dlg_Dn_List::get_total_bytes() const
{
	return m_totBytes;
}

bool Dlg_Dn_List::_download_list(const wstring& marker)
{
	wstring lnk = str::format(L"%s/?delimiter=/&prefix=Win/", BASE_URL);
	if (!marker.empty()) {
		lnk.append(L"&marker=").append(marker);
	}

	download dl(m_session, lnk);
	dl.set_referrer(REFERRER);
	dl.add_request_header({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err;
	if (!dl.start(&err)) {
		return show_err_and_close(L"Error at download start", err);
	}
	on_ui_thread([&]() {
		m_progBar.set_pos(0);
		m_taskbarProg.set_waiting(true);
		m_lblTitle.set_text(L"XML download started...");
	});

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.get_content_length());
	while (dl.has_data(&err)) {
		xmlbuf.insert(xmlbuf.end(),
			dl.get_buffer().begin(), dl.get_buffer().end()); // append
		on_ui_thread([&]() {
			m_progBar.set_pos(dl.get_percent());
			m_lblTitle.set_text( str::format(L"%.2f%% downloaded (%.2f KB)...\n",
				dl.get_percent(),
				static_cast<float>(dl.get_total_downloaded()) / 1024) );
		});
	}

	if (!err.empty()) {
		return show_err_and_close(L"Download error", err);
	}
	return _read_xml(xmlbuf);
}

bool Dlg_Dn_List::_read_xml(const vector<BYTE>& buf)
{
	wstring xmlStr, err;
	if (!str::parse_blob(buf, xmlStr, &err)) {
		return show_err_and_close(L"XML parsing error", err);
	}
	xml xmlc = xmlStr;
	m_clist.append(xmlc);
	m_totBytes += static_cast<int>(buf.size());
	on_ui_thread([&]() {
		SetWindowText(hwnd(), str::format(L"%d markers downloaded (%.2f KB)...",
			m_clist.markers().size(),
			static_cast<float>(m_totBytes) / 1024).c_str() );
	});
	
	if (!m_clist.is_finished()) {
		on_ui_thread([&]() {
			m_lblTitle.set_text( str::format(L"Next marker: %s...\n", m_clist.next_marker()) );
		});
		_download_list(m_clist.next_marker()); // proceed to next marker
	} else {
		on_ui_thread([&]() {
			m_taskbarProg.clear();
			EndDialog(hwnd(), IDOK); // all markers downloaded
		});
	}
	return true;
}