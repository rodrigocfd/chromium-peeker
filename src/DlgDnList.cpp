
#include "DlgDnList.h"
#include "../res/resource.h"

DlgDnList::DlgDnList(Internet::Session& isess, ChromiumRel& list)
	: session(isess), clist(list), totBytes(0)
{
}

void DlgDnList::events()
{
	this->defineDialog(DLG_PROGRESS);

	this->onInitDialog([&]() {
		this->setXButton(false);
		this->setText(L"No markers downloaded...");
	
		( this->label = this->getChild(LBL_LBL) )
			.setText(L"Waiting...");
	
		( this->progBar = this->getChild(PRO_PRO) )
			.setRange(0, 100);

		System::Thread([&]() {
			this->doDownloadList(L""); // start downloading first batch of markers
		});
	});
}

bool DlgDnList::doDownloadList(const wstring& marker)
{
	wstring lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=Win/";
	if (!marker.empty())
		lnk.append(L"&marker=").append(marker);	

	Internet::Download dl(this->session, lnk);
	dl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dl.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err;
	if (!dl.start(&err))
		return this->doShowErrAndClose(L"Error at download start", err);
	this->sendFunction([&]() {
		this->progBar.setPos(0);
		this->label.setText(L"XML download started...");
	});

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.getContentLength());
	while (dl.hasData(&err)) {
		xmlbuf.insert(xmlbuf.end(), dl.getBuffer().begin(), dl.getBuffer().end());
		this->sendFunction([&]() {
			this->progBar.setPos((int)dl.getPercent());
			this->label.setText( Sprintf(L"%.2f%% downloaded (%d bytes)...\n",
				dl.getPercent(), dl.getTotalDownloaded()) );
		});
	}

	if (!err.empty())
		return this->doShowErrAndClose(L"Download error", err);

	return this->doReadXml(xmlbuf);
}

bool DlgDnList::doReadXml(const vector<BYTE>& buf)
{
	Xml xml = ParseUtf8(buf);
	this->clist.append(xml);
	this->totBytes += static_cast<int>(buf.size());
	this->sendFunction([&]() {
		this->setText( Sprintf(L"%d markers downloaded (%.2f KB)...",
			this->clist.markers().size(), (float)this->totBytes / 1024) );
	});
	
	if (!this->clist.isFinished()) {
		this->sendFunction([&]() {
			this->label.setText( Sprintf(L"Next marker: %s...\n", this->clist.nextMarker()) );
		});
		this->doDownloadList(this->clist.nextMarker()); // proceed to next marker
	} else {
		this->sendFunction([&]() { this->endDialog(IDOK); }); // all markers downloaded
	}
	return true;
}

bool DlgDnList::doShowErrAndClose(const wchar_t *msg, const wstring& err)
{
	this->sendFunction([&]() {
		this->messageBox(msg, err, MB_ICONERROR);
		this->endDialog(IDCANCEL);
	});
	return false;
}