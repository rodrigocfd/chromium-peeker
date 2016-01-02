
#include "FrmDnList.h"
using namespace wolf;
using std::vector;
using std::wstring;

FrmDnList::FrmDnList(TaskBarProgress& taskBar,
	InternetSession& session,
	ChromiumRel& clist)
	: FrmDn(taskBar), _session(session), _clist(clist), _totBytes(0)
{
	this->onMessage(WM_CREATE, [&](WPARAM wp, LPARAM lp)->LRESULT
	{
		this->setText(L"No markers downloaded...");
		Sys::thread([this]() {
			_doDownloadList(L""); // start downloading first batch of markers
		});
		return 0;
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
		return _doShowErrAndClose(L"Error at download start", err);
	}
	this->guiThread([this]()->void {
		_progBar.setPos(0);
		_taskBar.setWaiting(true);
		_label.setText(L"XML download started...");
	});

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.getContentLength());
	while (dl.hasData(&err)) {
		xmlbuf.insert(xmlbuf.end(), dl.getBuffer().begin(), dl.getBuffer().end()); // append
		this->guiThread([&]()->void {
			_progBar.setPos(dl.getPercent());
			_label.setText( Str::format(L"%.2f%% downloaded (%.2f KB)...\n",
				dl.getPercent(), static_cast<float>(dl.getTotalDownloaded()) / 1024) );
		});
	}

	if (!err.empty()) {
		return _doShowErrAndClose(L"Download error", err);
	}
	return _doReadXml(xmlbuf);
}

bool FrmDnList::_doReadXml(const vector<BYTE>& buf)
{
	Xml xml = Str::parseUtf8(buf);
	_clist.append(xml);
	_totBytes += static_cast<int>(buf.size());
	this->guiThread([this]()->void {
		this->setText( Str::format(L"%d markers downloaded (%.2f KB)...",
			_clist.markers().size(), static_cast<float>(_totBytes) / 1024) );
	});
	
	if (!_clist.isFinished()) {
		this->guiThread([this]()->void {
			_label.setText( Str::format(L"Next marker: %s...\n", _clist.nextMarker()) );
		});
		_doDownloadList(_clist.nextMarker()); // proceed to next marker
	} else {
		this->guiThread([this]()->void {
			_taskBar.dismiss();
			this->sendMessage(WM_CLOSE, 0, 0); // all markers downloaded
		});
	}
	return true;
}