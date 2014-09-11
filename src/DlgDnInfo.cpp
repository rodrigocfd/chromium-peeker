
#include "DlgDnInfo.h"
#include "../res/resource.h"

int DlgDnInfo::show(Window *parent, Internet::Session *session, const Array<String> *pMarkers)
{
	this->pSession = session;
	this->pMarkers = pMarkers;
	this->totDownloaded = 0;
	return DialogModal::show(parent, DLG_PROGRESS);
}

INT_PTR DlgDnInfo::msgHandler(UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
	case WM_INITDIALOG: this->onInitDialog(); break;
	}
	return DialogModal::msgHandler(msg, wp, lp);
}

void DlgDnInfo::onInitDialog()
{
	this->setXButton(false);
	this->setText(L"Downloading...");

	( this->label = this->getChild(LBL_LBL) )
		.setText(L"Waiting...");

	( this->progBar = this->getChild(PRO_PRO) )
		.setRange(0, 100)
		.setPos(0);

	this->doGetOneFile( (*this->pMarkers)[0].str() ); // proceed with first file
}

void DlgDnInfo::doGetOneFile(const wchar_t *marker)
{
	String lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=";
	lnk.append(marker);
	
	Internet::Download dl(*this->pSession, this);
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
			this->messageBox(L"Download error",
				String::Fmt(L"Download failed.\n%s", s->err.str()),
				MB_ICONERROR);
			this->endDialog(IDCANCEL);
			break;
		case Internet::Msg::BEGIN:
			this->label.setText( String::Fmt(L"Downloading file %d/%d...",
				this->data.size() + 1, this->pMarkers->size()) );
			/*for(int i = 0; i < pStatus->responseHeader.size(); ++i)
				dbg(L"[%s] [%s]\n",
					pStatus->responseHeader.at(i)->key.str(),
					pStatus->responseHeader.at(i)->val.str() );*/
			break;
		/*case Internet::Msg::PROGRESS:
			this->label.setText( String::Fmt(L"%.2f%% downloaded (%d bytes)...\n", s->pct(), s->totalDownloaded) );
			this->progBar.setPos( Rounds(s->pct()) );
			break;*/
		case Internet::Msg::END:
			this->progBar.setPos( ((float)this->data.size() / this->pMarkers->size()) * 100 );
			this->doProcessFile(s->buffer);
		}
	});
}

void DlgDnInfo::doProcessFile(const Array<BYTE>& buf)
{
	this->totDownloaded += buf.size();
	this->setText( String::Fmt(L"Downloading (%.2f KB)...", (float)this->totDownloaded / 1024) );

	Xml xml = String::ParseUtf8(buf).str();
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
		this->doGetOneFile( (*this->pMarkers)[this->data.size()].str() ); // proceed to next file
	}
}