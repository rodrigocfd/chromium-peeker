
#include "DlgDownloadInfo.h"
#include "../toolow/Internet.h"
#include "../toolow/Xml.h"
#include "../res/resource.h"

int DlgDownloadInfo::show(Window *parent, const Array<String> *pMarkers)
{
	this->pMarkers = pMarkers;
	this->totDownloaded = 0;
	return DialogModal::show(parent, DLG_PROGRESS);
}

INT_PTR DlgDownloadInfo::msgHandler(UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
	case WM_INITDIALOG: on_initDialog(); break;
	case WM_APP+1:      on_webEvent(wp, lp); break;
	}
	return DialogModal::msgHandler(msg, wp, lp);
}

void DlgDownloadInfo::on_initDialog()
{
	this->setXButton(false);
	this->setText(L"Downloading...");

	( this->label = this->getChild(LBL_LBL) )
		.setText(L"Waiting...");

	( this->progBar = this->getChild(PRO_PRO) )
		.setRange(0, 100)
		.setPos(0);

	do_getOneFile( (*this->pMarkers)[0].str() ); // proceed with first file
}

void DlgDownloadInfo::on_webEvent(WPARAM wp, LPARAM lp)
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
		this->label.setText( FMT(L"Downloading file %d/%d...",
			this->data.size() + 1, this->pMarkers->size()) );
		/*for(int i = 0; i < pStatus->responseHeader.size(); ++i)
			dbg(L"[%s] [%s]\n",
				pStatus->responseHeader.at(i)->key.str(),
				pStatus->responseHeader.at(i)->val.str() );*/
		break;
	/*case Internet::Status::Flag::PROGRESS:
		this->label.setText( FMT(L"%.2f%% downloaded (%d bytes)...\n", pStatus->pctDone, pStatus->buffer.size()) );
		this->progBar.setPos((int)pStatus->pctDone * 100);
		break;*/
	case Internet::Status::Flag::DONE:
		this->progBar.setPos( ((float)this->data.size() / this->pMarkers->size()) * 100 );
		//this->label.setText( FMT(L"XML download finished (%d bytes).\n", pStatus->buffer.size()) );
		do_processFile(&pStatus->buffer);
	}
}

void DlgDownloadInfo::do_getOneFile(const wchar_t *marker)
{
	String lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=";
	lnk.append(marker);
	
	Internet dl;
	dl.addRequestHeader(L"Accept-Encoding: gzip,deflate,sdch")
		.addRequestHeader(L"Connection: keep-alive")
		.addRequestHeader(L"DNT: 1")
		.addRequestHeader(L"Host: commondatastorage.googleapis.com")
		.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/")
		.registerNotify(this->hWnd(), WM_APP + 1)
		.download(lnk.str());
}

void DlgDownloadInfo::do_processFile(const Array<BYTE> *pData)
{
	this->totDownloaded += pData->size();
	this->setText( FMT(L"Downloading (%.2f KB)...", (float)this->totDownloaded / 1024) );

	Xml xml = String::ParseUtf8(&(*pData)[0], pData->size()).str();
	this->data.realloc( this->data.size() + 1 ); // realloc public return buffer

	Array<Xml::Node*> cnodes = xml.root.getChildrenByName(L"Contents");
	for(int i = 0; i < cnodes.size(); ++i) {
		if(cnodes[i]->firstChildByName(L"Key")->value.endsWith(L"chrome-win32.zip", String::Case::SENS)) {
			this->data.last().releaseDate = cnodes[i]->firstChildByName(L"LastModified")->value;
			this->data.last().packageSize = cnodes[i]->firstChildByName(L"Size")->value.toInt();
			break;
		}
	}

	if(this->data.size() == this->pMarkers->size()) {
		this->endDialog(IDOK); // last file has been processed
	} else {
		do_getOneFile( (*this->pMarkers)[this->data.size()].str() ); // proceed to next file
	}
}