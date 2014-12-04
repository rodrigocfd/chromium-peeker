
#include "DlgDnList.h"
#include "../res/resource.h"

int DlgDnList::show(Window *parent, Internet::Session *session, ChromiumRel *list)
{
	this->pSession = session;
	this->pCList = list;
	this->totBytes = 0;
	return DialogModal::show(parent, DLG_PROGRESS);
}

INT_PTR DlgDnList::msgHandler(UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_INITDIALOG: this->onInitDialog(); break;
	}
	return DialogModal::msgHandler(msg, wp, lp);
}

void DlgDnList::onInitDialog()
{
	this->setXButton(false);
	this->setText(L"No markers downloaded...");
	
	( this->label = this->getChild(LBL_LBL) )
		.setText(L"Waiting...");
	
	( this->progBar = this->getChild(PRO_PRO) )
		.setRange(0, 100);

	System::Thread([&]() {
		this->doDownloadList(L""); // start downloading first batch of markers
	});
}

bool DlgDnList::doDownloadList(const wchar_t *marker)
{
	String lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=Win/";
	if (*marker) lnk.append(L"&marker=").append(marker);	

	Internet::Download dl(*pSession, lnk);
	dl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dl.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	String err;
	if (!dl.start(&err))
		return this->doShowErrAndClose(L"Error at download start", err);
	this->sendFunction([&]() {
		this->progBar.setPos(0);
		this->label.setText(L"XML download started...");
	});

	Array<BYTE> xmlbuf;
	xmlbuf.reserve(dl.getContentLength());
	while (dl.hasData(&err)) {
		xmlbuf.append(dl.getBuffer());
		this->sendFunction([&]() {
			this->progBar.setPos((int)dl.getPercent());
			this->label.setText( String::Fmt(L"%.2f%% downloaded (%d bytes)...\n",
				dl.getPercent(), dl.getTotalDownloaded()) );
		});
	}

	if (!err.isEmpty())
		return this->doShowErrAndClose(L"Download error", err);

	return this->doReadXml(xmlbuf);
}

bool DlgDnList::doReadXml(const Array<BYTE>& buf)
{
	Xml xml = String::ParseUtf8(buf);
	this->pCList->append(xml);
	this->totBytes += buf.size();
	this->sendFunction([=]() {
		this->setText( String::Fmt(L"%d markers downloaded (%.2f KB)...",
			this->pCList->markers().size(), (float)this->totBytes / 1024) );
	});
	
	if (!this->pCList->isFinished()) {
		this->sendFunction([&]() {
			this->label.setText( String::Fmt(L"Next marker: %s...\n", this->pCList->nextMarker()) );
		});
		this->doDownloadList(this->pCList->nextMarker());
	} else {
		this->sendFunction([&]() { this->endDialog(IDOK); }); // all markers downloaded
	}
	return true;
}

bool DlgDnList::doShowErrAndClose(const wchar_t *msg, const String& err)
{
	this->sendFunction([&]() {
		this->messageBox(msg, err, MB_ICONERROR);
		this->endDialog(IDCANCEL);
	});
	return false;
}