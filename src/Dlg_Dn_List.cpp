
#include "Dlg_Dn_List.h"
#include "../wet/str.h"
#include "../wet/sys.h"
#include "../wet/xml.h"
using namespace wet;
using std::vector;
using std::wstring;

Dlg_Dn_List::Dlg_Dn_List(progress_taskbar& taskBar, download::session& session, Chromium_Rel& clist)
	: Dlg_Dn(taskBar), _session(session), _clist(clist), _totBytes(0)
{
}

INT_PTR Dlg_Dn_List::proc(params p)
{
	if (p.message == WM_INITDIALOG) {
		init_controls();
		set_caption(L"No markers downloaded...");
		sys::thread([&]() {
			_download_list(L""); // start downloading first batch of markers
		});
		return TRUE;
	}

	return def_proc(p);
}

int Dlg_Dn_List::get_total_bytes() const
{
	return _totBytes;
}

bool Dlg_Dn_List::_download_list(const wstring& marker)
{
	wstring lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=Win/";
	if (!marker.empty()) {
		lnk.append(L"&marker=").append(marker);
	}

	download dl(_session, lnk);
	dl.set_referrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
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
	ui_thread([&]() {
		_progBar.set_pos(0);
		_taskBar.set_waiting(true);
		_label.set_text(L"XML download started...");
	});

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.get_content_length());
	while (dl.has_data(&err)) {
		xmlbuf.insert(xmlbuf.end(),
			dl.get_buffer().begin(), dl.get_buffer().end()); // append
		ui_thread([&]() {
			_progBar.set_pos(dl.get_percent());
			_label.set_text( str::format(L"%.2f%% downloaded (%.2f KB)...\n",
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
	_clist.append(xmlc);
	_totBytes += static_cast<int>(buf.size());
	ui_thread([&]() {
		set_caption(str::format(L"%d markers downloaded (%.2f KB)...",
			_clist.markers().size(),
			static_cast<float>(_totBytes) / 1024).c_str() );
	});
	
	if (!_clist.is_finished()) {
		ui_thread([&]() {
			_label.set_text( str::format(L"Next marker: %s...\n", _clist.next_marker()) );
		});
		_download_list(_clist.next_marker()); // proceed to next marker
	} else {
		ui_thread([&]() {
			_taskBar.clear();
			EndDialog(hwnd(), IDOK); // all markers downloaded
		});
	}
	return true;
}