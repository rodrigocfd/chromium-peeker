
#include "FrmDnList.h"
#include "../winutil/str.h"
#include "../winutil/sys.h"
#include "../winutil/xml.h"
using namespace winutil;
using std::vector;
using std::wstring;

FrmDnList::FrmDnList(taskbar_progress& taskBar,
	internet_session& session,
	ChromiumRel& clist)
	: FrmDn(taskBar), _session(session), _clist(clist), _totBytes(0)
{
	on_message(WM_INITDIALOG, [this](params p)->INT_PTR
	{
		initControls();
		SetWindowText(hwnd(), L"No markers downloaded...");
		sys::thread([this]() {
			_doDownloadList(L""); // start downloading first batch of markers
		});
		return TRUE;
	});
}

int FrmDnList::getTotalBytes() const
{
	return _totBytes;
}

bool FrmDnList::_doDownloadList(const wstring& marker)
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
		return doShowErrAndClose(L"Error at download start", err);
	}
	ui_thread([this]()->void {
		_progBar.set_pos(0);
		_taskBar.set_waiting(true);
		_label.set_text(L"XML download started...");
	});

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.get_content_length());
	while (dl.has_data(&err)) {
		xmlbuf.insert(xmlbuf.end(), dl.get_buffer().begin(), dl.get_buffer().end()); // append
		ui_thread([&]()->void {
			_progBar.set_pos(dl.get_percent());
			_label.set_text( str::format(L"%.2f%% downloaded (%.2f KB)...\n",
				dl.get_percent(), static_cast<float>(dl.get_total_downloaded()) / 1024) );
		});
	}

	if (!err.empty()) {
		return doShowErrAndClose(L"Download error", err);
	}
	return _doReadXml(xmlbuf);
}

bool FrmDnList::_doReadXml(const vector<BYTE>& buf)
{
	xml xmlc = str::parse_utf8(buf);
	_clist.append(xmlc);
	_totBytes += static_cast<int>(buf.size());
	ui_thread([this]()->void {
		SetWindowText(hwnd(), str::format(L"%d markers downloaded (%.2f KB)...",
			_clist.markers().size(), static_cast<float>(_totBytes) / 1024).c_str() );
	});
	
	if (!_clist.isFinished()) {
		ui_thread([this]()->void {
			_label.set_text( str::format(L"Next marker: %s...\n", _clist.nextMarker()) );
		});
		_doDownloadList(_clist.nextMarker()); // proceed to next marker
	} else {
		ui_thread([this]()->void {
			_taskBar.clear();
			EndDialog(hwnd(), IDOK); // all markers downloaded
		});
	}
	return true;
}