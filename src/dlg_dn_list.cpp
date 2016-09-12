
#include "dlg_dn_list.h"
#include "../winutil/str.h"
#include "../winutil/sys.h"
#include "../winutil/xml.h"
using namespace winutil;
using std::vector;
using std::wstring;

dlg_dn_list::dlg_dn_list(taskbar_progress& taskBar,
	internet_session& session,
	chromium_rel& clist)
	: dlg_dn(taskBar), _session(session), _clist(clist), _totBytes(0)
{
	on.INITDIALOG([this](par::initdialog p)->INT_PTR
	{
		init_controls();
		SetWindowText(hwnd(), L"No markers downloaded...");
		sys::thread([this]()->void {
			_download_list(L""); // start downloading first batch of markers
		});
		return TRUE;
	});
}

int dlg_dn_list::get_total_bytes() const
{
	return _totBytes;
}

bool dlg_dn_list::_download_list(const wstring& marker)
{
	wstring lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=Win/";
	if (!marker.empty()) {
		lnk.append(L"&marker=").append(marker);
	}

	internet_download dl(_session, lnk);
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
	on_ui_thread([this]()->void {
		_progBar.set_pos(0);
		_taskBar.set_waiting(true);
		_label.set_text(L"XML download started...");
	});

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.get_content_length());
	while (dl.has_data(&err)) {
		xmlbuf.insert(xmlbuf.end(), dl.get_buffer().begin(), dl.get_buffer().end()); // append
		on_ui_thread([&]()->void {
			_progBar.set_pos(dl.get_percent());
			_label.set_text( str::format(L"%.2f%% downloaded (%.2f KB)...\n",
				dl.get_percent(), static_cast<float>(dl.get_total_downloaded()) / 1024) );
		});
	}

	if (!err.empty()) {
		return show_err_and_close(L"Download error", err);
	}
	return _read_xml(xmlbuf);
}

bool dlg_dn_list::_read_xml(const vector<BYTE>& buf)
{
	xml xmlc = str::parse_utf8(buf);
	_clist.append(xmlc);
	_totBytes += static_cast<int>(buf.size());
	on_ui_thread([this]()->void {
		SetWindowText(hwnd(), str::format(L"%d markers downloaded (%.2f KB)...",
			_clist.markers().size(), static_cast<float>(_totBytes) / 1024).c_str() );
	});
	
	if (!_clist.is_finished()) {
		on_ui_thread([this]()->void {
			_label.set_text( str::format(L"Next marker: %s...\n", _clist.next_marker()) );
		});
		_download_list(_clist.next_marker()); // proceed to next marker
	} else {
		on_ui_thread([this]()->void {
			_taskBar.clear();
			EndDialog(hwnd(), IDOK); // all markers downloaded
		});
	}
	return true;
}