
#include "FrmDnInfo.h"
#include "../winutil/Str.h"
#include "../winutil/Sys.h"
#include "../winutil/Xml.h"
using std::vector;
using std::wstring;

FrmDnInfo::FrmDnInfo(TaskBarProgress& taskBar,
	InternetSession& session,
	const vector<wstring>& markers)
	: FrmDn(taskBar), _session(session), _markers(markers), _totDownloaded(0)
{
	on_message(WM_INITDIALOG, [this](WPARAM wp, LPARAM lp)->INT_PTR
	{
		SetWindowText(hwnd(), L"Downloading...");
		Sys::thread([this]() {
			_doGetOneFile(_markers[0]); // proceed with first file
		});
		return TRUE;
	});
}

bool FrmDnInfo::_doGetOneFile(const wstring& marker)
{
	wstring lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=";
	lnk.append(marker);

	InternetDownload dl(_session, lnk);
	dl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dl.addRequestHeader({
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
	xmlbuf.reserve(dl.getContentLength());
	while (dl.hasData(&err)) { // each file is small, we don't need to display progress info
		xmlbuf.insert(xmlbuf.end(), dl.getBuffer().begin(), dl.getBuffer().end()); // append
	}

	if (!err.empty()) {
		return doShowErrAndClose(L"Download error", err);
	}

	return _doProcessFile(xmlbuf);
}

bool FrmDnInfo::_doProcessFile(const vector<BYTE>& buf)
{
	_totDownloaded += static_cast<int>(buf.size());
	gui_thread([this]()->void {
		_label.setText( Str::format(L"%d/%d markers (%.2f KB)...",
			data.size(), _markers.size(), static_cast<float>(_totDownloaded) / 1024) );
		double pct = (static_cast<float>(this->data.size()) / _markers.size()) * 100;
		_progBar.setPos(pct);
		_taskBar.setPos(pct);
	});

	Xml xml = Str::parseUtf8(buf);
	this->data.resize( this->data.size() + 1 ); // realloc public return buffer

	vector<Xml::Node*> cnodes = xml.root.getChildrenByName(L"Contents");
	for (Xml::Node *cnode : cnodes) {
		if (Str::endsWithI(cnode->firstChildByName(L"Key")->value, L"chrome-win32.zip")) {
			this->data.back().releaseDate = cnode->firstChildByName(L"LastModified")->value;
			this->data.back().packageSize = std::stoi(cnode->firstChildByName(L"Size")->value);
			break;
		}
	}

	if (data.size() == _markers.size()) {
		gui_thread([this]()->void {
			_taskBar.dismiss();
			EndDialog(hwnd(), IDOK); // last file has been processed
		});
	} else {
		_doGetOneFile( _markers[this->data.size()].c_str() ); // proceed to next file
	}
	return true;
}