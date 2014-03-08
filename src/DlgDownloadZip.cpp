
#include "DlgDownloadZip.h"
#include "../toolow/Internet.h"
#include "../toolow/Xml.h"
#include "../toolow/File.h"
#include "../res/resource.h"

int DlgDownloadZip::show(Window *parent, const wchar_t *marker)
{
	this->marker = marker;
	return DialogModal::show(parent, DLG_PROGRESS);
}

INT_PTR DlgDownloadZip::msgHandler(UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
	case WM_INITDIALOG: on_initDialog(); break;
	case WM_APP+1:      on_webEvent(wp, lp); break;
	}
	return DialogModal::msgHandler(msg, wp, lp);
}

void DlgDownloadZip::on_initDialog()
{
	this->setXButton(false);
	this->setText(L"Downloading chrome-win32.zip...");

	( this->label = this->getChild(LBL_LBL) )
		.setText(L"Waiting...");

	( this->progBar = this->getChild(PRO_PRO) )
		.setRange(0, 100)
		.setPos(0);

	if(this->getFileSave(L"Zip file (*.zip)\0*.zip\0", &this->dest)) {
		Internet dl;
		dl.addRequestHeader(L"Accept-Encoding: gzip,deflate,sdch")
			.addRequestHeader(L"Connection: keep-alive")
			.addRequestHeader(L"DNT: 1")
			.addRequestHeader(L"Host: commondatastorage.googleapis.com")
			.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/")
			.registerNotify(this->hWnd(), WM_APP + 1)
			.download( FMT(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip", this->marker.str()) );
	} else {
		this->endDialog(IDCANCEL);
	}
}

void DlgDownloadZip::on_webEvent(WPARAM wp, LPARAM lp)
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
	/*case Internet::Status::Flag::STARTED:
		for(int i = 0; i < pStatus->responseHeader.size(); ++i)
			dbg(L"[%s] [%s]\n",
				pStatus->responseHeader.at(i)->key.str(),
				pStatus->responseHeader.at(i)->val.str() );
		break;*/
	case Internet::Status::Flag::PROGRESS:
		this->label.setText(FMT(L"%.0f%% downloaded (%.1f MB)...\n",
			pStatus->pctDone * 100, (float)pStatus->buffer.size() / 1024 / 1024 ));
		this->progBar.setPos(pStatus->pctDone * 100);
		break;
	case Internet::Status::Flag::DONE:
		//this->progBar.setPos(rounds( ((float)this->data.size() / this->pMarkers->size()) * 100 ));
		//this->label.setText( FMT(L"XML download finished (%d bytes).\n", pStatus->buffer.size()) );
		//do_processFile(&pStatus->buffer);
		{
			File::Raw fout;
			fout.open(this->dest.str(), File::Access::READWRITE);
			fout.write(&pStatus->buffer);
			this->endDialog(IDOK);
		}
		break;
	}
}