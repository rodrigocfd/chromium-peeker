
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

	System::Thread([&]() {
		this->doGetOneFile( (*this->pMarkers)[0].str() ); // proceed with first file
	});
}

bool DlgDnInfo::doGetOneFile(const wchar_t *marker)
{
	String lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=";
	lnk.append(marker);

	Internet::Download dl(*this->pSession, lnk);
	dl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dl.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	String err;
	if(!dl.start(&err))
		return this->doShowErrAndClose(L"Error at download start", err);

	Array<BYTE> xmlbuf;
	xmlbuf.reserve(dl.getContentLength());
	while(dl.hasData(&err)) // each file is small, we don't need to display progress info
		xmlbuf.append(dl.getBuffer());

	if(!err.isEmpty())
		return this->doShowErrAndClose(L"Download error", err);

	return this->doProcessFile(xmlbuf);
}

bool DlgDnInfo::doProcessFile(const Array<BYTE>& buf)
{
	this->totDownloaded += buf.size();
	this->sendFunction([&]() {
		this->label.setText( String::Fmt(L"%d/%d markers (%.2f KB)...",
			this->data.size(), this->pMarkers->size(), (float)this->totDownloaded / 1024) );
		this->progBar.setPos( ((float)this->data.size() / this->pMarkers->size()) * 100 );
	});

	Xml xml = String::ParseUtf8(buf);
	this->data.resize( this->data.size() + 1 ); // realloc public return buffer

	Array<Xml::Node*> cnodes = xml.root.getChildrenByName(L"Contents");
	for(int i = 0; i < cnodes.size(); ++i) {
		if(cnodes[i]->firstChildByName(L"Key")->value.endsWithCS(L"chrome-win32.zip")) {
			this->data.last().releaseDate = cnodes[i]->firstChildByName(L"LastModified")->value;
			this->data.last().packageSize = cnodes[i]->firstChildByName(L"Size")->value.toInt();
			break;
		}
	}

	if(this->data.size() == this->pMarkers->size()) {
		this->sendFunction([&]() { this->endDialog(IDOK); }); // last file has been processed
	} else {
		this->doGetOneFile( (*this->pMarkers)[this->data.size()].str() ); // proceed to next file
	}
	return true;
}

bool DlgDnInfo::doShowErrAndClose(const wchar_t *msg, const String& err)
{
	this->sendFunction([&]() {
		this->messageBox(msg, err, MB_ICONERROR);
		this->endDialog(IDCANCEL);
	});
	return false;
}