
#include "DlgDownloadList.h"
#include "../toolow/Internet.h"
#include "../toolow/Xml.h"
#include "../res/resource.h"

int DlgDownloadList::show(Window *parent, ChromiumReleaseList *pList)
{
	this->pCList = pList;
	this->totBytes = 0;
	return DialogModal::show(parent, DLG_PROGRESS);
}

INT_PTR DlgDownloadList::msgHandler(UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
	case WM_INITDIALOG: on_initDialog(); break;
	case WM_APP+1:      on_webEvent(wp, lp); break;
	}
	return DialogModal::msgHandler(msg, wp, lp);
}

void DlgDownloadList::on_initDialog()
{
	this->setXButton(false);
	this->setText(L"No markers downloaded...");
	
	( this->label = this->getChild(LBL_LBL) )
		.setText(L"Waiting...");
	
	( this->progBar = this->getChild(PRO_PRO) )
		.setRange(0, 100);

	do_downloadList(L""); // start downloading first batch of markers
}

void DlgDownloadList::on_webEvent(WPARAM wp, LPARAM lp)
{
	const Internet::Status *pStatus = (const Internet::Status*)lp;
	
	switch(pStatus->flag)
	{
	case Internet::Status::Flag::FAILED:
		this->messageBox(L"Download error",
			FMT(L"Download failed.\n%s", pStatus->msg.str()),
			MB_ICONERROR);
		this->endDialog(IDCANCEL);
		break;
	case Internet::Status::Flag::STARTED:
		this->progBar.setPos(0);
		this->label.setText(L"XML download started...");
		/*for(int i = 0; i < pStatus->responseHeader.size(); ++i)
			dbg(L"[%s] [%s]\n",
				pStatus->responseHeader.at(i)->key.str(),
				pStatus->responseHeader.at(i)->val.str() );*/
		break;
	case Internet::Status::Flag::PROGRESS:
		this->label.setText( FMT(L"%.2f%% downloaded (%d bytes)...\n", pStatus->pctDone, pStatus->buffer.size()) );
		this->progBar.setPos((int)pStatus->pctDone * 100);
		break;
	case Internet::Status::Flag::DONE:
		this->progBar.setPos(100);
		this->label.setText( FMT(L"XML download finished (%d bytes).\n", pStatus->buffer.size()) );
		do_readXml(&pStatus->buffer);
	}
}

void DlgDownloadList::do_downloadList(const wchar_t *marker)
{
	String lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=Win/";
	if(*marker) lnk.append(L"&marker=").append(marker);

	Internet dl;
	dl.addRequestHeader(L"Accept-Encoding: gzip,deflate,sdch")
		.addRequestHeader(L"Connection: keep-alive")
		.addRequestHeader(L"DNT: 1")
		.addRequestHeader(L"Host: commondatastorage.googleapis.com")
		.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/")
		.registerNotify(this->hWnd(), WM_APP + 1)
		.download(lnk.str());
}

void DlgDownloadList::do_readXml(const Array<BYTE> *buf)
{
	Xml xml = String::ParseUtf8(&(*buf)[0], buf->size()).str();
	this->pCList->append(&xml);
	this->totBytes += buf->size();
	this->setText( FMT(L"%d markers downloaded (%.2f KB)...",
		this->pCList->markers()->size(), (float)this->totBytes / 1024) );
	
	if(!this->pCList->isFinished()) {
		this->label.setText( FMT(L"Next marker: %s...\n", this->pCList->nextMarker()) );
		do_downloadList(this->pCList->nextMarker());
	} else {
		this->endDialog(IDOK); // all markers downloaded
	}
}