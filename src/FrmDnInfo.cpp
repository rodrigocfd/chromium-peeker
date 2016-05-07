
#include "FrmDnInfo.h"
#include "../winutil/str.h"
#include "../winutil/sys.h"
#include "../winutil/xml.h"
using namespace winutil;
using std::vector;
using std::wstring;

FrmDnInfo::FrmDnInfo(taskbar_progress& taskBar,
	internet_session& session,
	const vector<wstring>& markers)
	: FrmDn(taskBar), _session(session), _markers(markers), _totDownloaded(0)
{
	on_message(WM_INITDIALOG, [this](params p)->INT_PTR
	{
		initControls();
		SetWindowText(hwnd(), L"Downloading...");
		sys::thread([this]() {
			_doGetOneFile(_markers[0]); // proceed with first file
		});
		return TRUE;
	});
}

bool FrmDnInfo::_doGetOneFile(const wstring& marker)
{
	wstring lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=";
	lnk.append(marker);

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

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.get_content_length());
	while (dl.has_data(&err)) { // each file is small, we don't need to display progress info
		xmlbuf.insert(xmlbuf.end(), dl.get_buffer().begin(), dl.get_buffer().end()); // append
	}

	if (!err.empty()) {
		return doShowErrAndClose(L"Download error", err);
	}

	return _doProcessFile(xmlbuf);
}

bool FrmDnInfo::_doProcessFile(const vector<BYTE>& buf)
{
	_totDownloaded += static_cast<int>(buf.size());
	ui_thread([this]()->void {
		_label.set_text( str::format(L"%d/%d markers (%.2f KB)...",
			data.size(), _markers.size(), static_cast<float>(_totDownloaded) / 1024) );
		double pct = (static_cast<float>(this->data.size()) / _markers.size()) * 100;
		_progBar.set_pos(pct);
		_taskBar.set_pos(pct);
	});

	xml xmlc = str::parse_utf8(buf);
	this->data.resize( this->data.size() + 1 ); // realloc public return buffer

	vector<xml::node*> cnodes = xmlc.root.children_by_name(L"Contents");
	for (xml::node *cnode : cnodes) {
		if (str::ends_withi(cnode->first_child_by_name(L"Key")->value, L"chrome-win32.zip")) {
			this->data.back().releaseDate = cnode->first_child_by_name(L"LastModified")->value;
			this->data.back().packageSize = std::stoi(cnode->first_child_by_name(L"Size")->value);
			break;
		}
	}

	if (data.size() == _markers.size()) {
		ui_thread([this]()->void {
			_taskBar.clear();
			EndDialog(hwnd(), IDOK); // last file has been processed
		});
	} else {
		_doGetOneFile( _markers[this->data.size()].c_str() ); // proceed to next file
	}
	return true;
}