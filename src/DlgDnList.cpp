
#include "DlgDnList.h"
using namespace wolf;
using namespace wolf::res;
using std::vector;
using std::wstring;

void DlgDnList::events()
{
	this->onMessage(WM_INITDIALOG, [&](WPARAM wp, LPARAM lp)->INT_PTR {
		DlgDn::_initCtrls();
		this->setText(L"No markers downloaded...");
		sys::Thread([&]() {
			this->_doDownloadList(L""); // start downloading first batch of markers
		});
		return TRUE;
	});
}

bool DlgDnList::_doDownloadList(const wstring& marker)
{
	wstring lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=Win/";
	if (!marker.empty()) {
		lnk.append(L"&marker=").append(marker);
	}

	net::Download dl(_session, lnk);
	dl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dl.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err;
	if (!dl.start(&err)) {
		return DlgDn::_doShowErrAndClose(L"Error at download start", err);
	}
	this->inOrigThread([&]() {
		_progBar.setPos(0);
		_label.setText(L"XML download started...");
	});

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.getContentLength());
	while (dl.hasData(&err)) {
		vec::Append(xmlbuf, dl.getBuffer());
		this->inOrigThread([&]() {
			_progBar.setPos(static_cast<int>(dl.getPercent()));
			_label.setText( str::Sprintf(L"%.2f%% downloaded (%.2f KB)...\n",
				dl.getPercent(), static_cast<float>(dl.getTotalDownloaded()) / 1024) );
		});
	}

	if (!err.empty()) {
		return DlgDn::_doShowErrAndClose(L"Download error", err);
	}
	return this->_doReadXml(xmlbuf);
}

bool DlgDnList::_doReadXml(const vector<BYTE>& buf)
{
	Xml xml = str::ParseUtf8(buf);
	_clist.append(xml);
	_totBytes += static_cast<int>(buf.size());
	this->inOrigThread([&]() {
		this->setText( str::Sprintf(L"%d markers downloaded (%.2f KB)...",
			_clist.markers().size(), static_cast<float>(_totBytes) / 1024) );
	});
	
	if (!_clist.isFinished()) {
		this->inOrigThread([&]() {
			_label.setText( str::Sprintf(L"Next marker: %s...\n", _clist.nextMarker()) );
		});
		this->_doDownloadList(_clist.nextMarker()); // proceed to next marker
	} else {
		this->inOrigThread([&]() {
			this->endDialog(IDOK); // all markers downloaded
		});
	}
	return true;
}