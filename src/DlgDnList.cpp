
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
	switch(msg)
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

	this->doDownloadList(L""); // start downloading first batch of markers
}

void DlgDnList::doDownloadList(const wchar_t *marker)
{
	String lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=Win/";
	if(*marker) lnk.append(L"&marker=").append(marker);

	Internet::Download dl(*pSession, this);
	dl.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});
	dl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dl.download(lnk, [&](Internet::Msg msgType, const Internet::Status *s) {
		switch(msgType) {
		case Internet::Msg::FAIL:
			this->messageBox(L"Download error", String::Fmt(L"Download failed.\n%s", s->err.str()), MB_ICONERROR);
			this->endDialog(IDCANCEL);
			break;
		case Internet::Msg::BEGIN:
			this->progBar.setPos(0);
			this->label.setText(L"XML download started...");
			/*for(int i = 0; i < pStatus->responseHeader.size(); ++i)
				dbg(L"[%s] [%s]\n",
					pStatus->responseHeader.at(i)->key.str(),
					pStatus->responseHeader.at(i)->val.str() );*/
			break;
		case Internet::Msg::PROGRESS:
			this->label.setText( String::Fmt(L"%.2f%% downloaded (%d bytes)...\n", s->pct(), s->totalDownloaded) );
			this->progBar.setPos((int)s->pct());
			break;
		case Internet::Msg::END:
			this->progBar.setPos(100);
			this->label.setText( String::Fmt(L"XML download finished (%d bytes).\n", s->totalDownloaded) );
			this->doReadXml(s->buffer);
		}
	});
}

void DlgDnList::doReadXml(const Array<BYTE>& buf)
{
	Xml xml = String::ParseUtf8(buf).str();
	this->pCList->append(xml);
	this->totBytes += buf.size();
	this->setText( String::Fmt(L"%d markers downloaded (%.2f KB)...",
		this->pCList->markers().size(), (float)this->totBytes / 1024) );
	
	if(!this->pCList->isFinished()) {
		this->label.setText( String::Fmt(L"Next marker: %s...\n", this->pCList->nextMarker()) );
		this->doDownloadList(this->pCList->nextMarker());
	} else {
		this->endDialog(IDOK); // all markers downloaded
	}
}