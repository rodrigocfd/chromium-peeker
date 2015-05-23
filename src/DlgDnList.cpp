
#include "DlgDnList.h"
#include "../res/resource.h"

void DlgDnList::onInitDialog()
{
	DlgDn::initCtrls();
	this->setText(L"No markers downloaded...");
	sys::Thread([&]() {
		this->doDownloadList(L""); // start downloading first batch of markers
	});
}

bool DlgDnList::doDownloadList(const wstring& marker)
{
	wstring lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=Win/";
	if (!marker.empty()) {
		lnk.append(L"&marker=").append(marker);
	}

	net::Download dl(this->session, lnk);
	dl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dl.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err;
	if (!dl.start(&err)) {
		return DlgDn::doShowErrAndClose(L"Error at download start", err);
	}
	this->sendFunction([&]() {
		this->progBar.setPos(0);
		this->label.setText(L"XML download started...");
	});

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.getContentLength());
	while (dl.hasData(&err)) {
		vec::Append(xmlbuf, dl.getBuffer());
		this->sendFunction([&]() {
			this->progBar.setPos(static_cast<int>(dl.getPercent()));
			this->label.setText( str::Sprintf(L"%.2f%% downloaded (%.2f KB)...\n",
				dl.getPercent(), static_cast<float>(dl.getTotalDownloaded()) / 1024) );
		});
	}

	if (!err.empty()) {
		return DlgDn::doShowErrAndClose(L"Download error", err);
	}
	return this->doReadXml(xmlbuf);
}

bool DlgDnList::doReadXml(const vector<BYTE>& buf)
{
	Xml xml = str::ParseUtf8(buf);
	this->clist.append(xml);
	this->totBytes += static_cast<int>(buf.size());
	this->sendFunction([&]() {
		this->setText( str::Sprintf(L"%d markers downloaded (%.2f KB)...",
			this->clist.markers().size(), static_cast<float>(this->totBytes) / 1024) );
	});
	
	if (!this->clist.isFinished()) {
		this->sendFunction([&]() {
			this->label.setText( str::Sprintf(L"Next marker: %s...\n", this->clist.nextMarker()) );
		});
		this->doDownloadList(this->clist.nextMarker()); // proceed to next marker
	} else {
		this->sendFunction([&]() { this->endDialog(IDOK); }); // all markers downloaded
	}
	return true;
}