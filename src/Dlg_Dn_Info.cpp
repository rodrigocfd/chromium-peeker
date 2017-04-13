
#include "Dlg_Dn_Info.h"
#include "../winlamb-more/str.h"
#include "../winlamb-more/sys.h"
#include "../winlamb-more/xml.h"
using namespace wl;
using std::vector;
using std::wstring;

Dlg_Dn_Info::Dlg_Dn_Info(progress_taskbar& tb, download::session& sess,
	const vector<wstring>& mk)
	: Dlg_Dn(tb), m_session(sess), m_markers(mk), m_totDownloaded(0)
{
	on_message(WM_INITDIALOG, [&](params&)
	{
		init_controls();
		set_text(L"Downloading...");
		sys::thread([&]() {
			_get_one_file(m_markers[0]); // proceed with first file
		});
		return TRUE;
	});
}

bool Dlg_Dn_Info::_get_one_file(const wstring& marker)
{
	wstring lnk = str::format(L"%s/?delimiter=/&prefix=%s",
		BASE_URL, marker.c_str() );

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

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.get_content_length());
	while (dl.has_data(&err)) { // each file is small, we don't need to display progress info
		xmlbuf.insert(xmlbuf.end(),
			dl.get_buffer().begin(), dl.get_buffer().end()); // append
	}

	if (!err.empty()) {
		return show_err_and_close(L"Download error", err);
	}

	return _process_file(xmlbuf);
}

bool Dlg_Dn_Info::_process_file(const vector<BYTE>& buf)
{
	m_totDownloaded += static_cast<int>(buf.size());
	ui_thread([&]() {
		m_lblTitle.set_text( str::format(L"%d/%d markers (%.2f KB)...",
			data.size(), m_markers.size(),
			static_cast<float>(m_totDownloaded) / 1024) );
		double pct = (static_cast<float>(data.size()) / m_markers.size()) * 100;
		m_progBar.set_pos(pct);
		m_taskbarProg.set_pos(pct);
	});

	wstring xmlStr, err;
	if (!str::parse_blob(buf, xmlStr, &err)) {
		return show_err_and_close(L"XML parsing error", err);
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
		ui_thread([&]() {
			m_taskbarProg.clear();
			EndDialog(hwnd(), IDOK); // last file has been processed
		});
	} else {
		_get_one_file( m_markers[data.size()].c_str() ); // proceed to next file
	}
	return true;
}