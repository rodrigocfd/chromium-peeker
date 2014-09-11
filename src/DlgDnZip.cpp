
#include "DlgDnZip.h"
#include "../res/resource.h"

int DlgDnZip::show(Window *parent, Internet::Session *session, const String& marker)
{
	this->pSession = session;
	this->marker = marker;
	return DialogModal::show(parent, DLG_PROGRESS);
}

INT_PTR DlgDnZip::msgHandler(UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
	case WM_INITDIALOG: onInitDialog(); break;
	}
	return DialogModal::msgHandler(msg, wp, lp);
}

void DlgDnZip::onInitDialog()
{
	this->setXButton(false);
	this->setText(L"Downloading chrome-win32.zip...");

	( this->label = this->getChild(LBL_LBL) )
		.setText(L"Waiting...");

	( this->progBar = this->getChild(PRO_PRO) )
		.setRange(0, 100)
		.setPos(0);

	String defSave = System::GetDesktopPath().append(L"\\chrome-win32.zip");
	this->getFileSave(L"Zip file (*.zip)|*.zip", &this->dest, defSave.str()) ?
		this->doDownload() : this->endDialog(IDCANCEL);
}

void DlgDnZip::doDownload()
{
	Internet::Download dlzip(*this->pSession, this);
	dlzip.setDestFile(this->dest);
	dlzip.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});
	dlzip.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");

	String lnk = String::Fmt(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip",
		this->marker.str() );

	dlzip.download(lnk, [&](Internet::Msg msgType, const Internet::Status *s) {
		switch(msgType) {
		case Internet::Msg::FAIL:
			this->messageBox(L"Download error", String::Fmt(L"Download failed.\n%s", s->err.str()), MB_ICONERROR);
			this->endDialog(IDCANCEL);
			break;
		case Internet::Msg::BEGIN:
			this->setText( String::Fmt(L"Downloading %s...", File::Path::GetFilename(this->dest)) );
			/*s->responseHeaders.each([](const Hash<String>::Elem& elem) {
				dbg(L"[%s] [%s]\n", elem.key.str(), elem.val.str() );
			});*/
			break;
		case Internet::Msg::PROGRESS:
			this->label.setText( String::Fmt(L"%.0f%% downloaded (%.1f MB)...\n",
				s->pct(), (float)s->totalDownloaded / 1024 / 1024 ));
			this->progBar.setPos((int)s->pct());
			break;
		case Internet::Msg::END:
			this->endDialog(IDOK);
		}
	});
}