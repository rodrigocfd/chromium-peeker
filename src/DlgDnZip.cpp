
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
	switch (msg)
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
	if (this->getFileSave(L"Zip file (*.zip)|*.zip", this->dest, defSave.str())) {
		System::Thread([&]() {
			this->doDownload();
		});
	} else {
		this->endDialog(IDCANCEL);
	}
}

bool DlgDnZip::doDownload()
{
	String lnk = String::Fmt(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip",
		this->marker.str() );

	Internet::Download zipdl(*this->pSession, lnk);
	zipdl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	zipdl.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	String err;
	File::Raw fout;
	if (!fout.open(this->dest, File::Access::READWRITE, &err))
		return this->doShowErrAndClose(L"File creation error", err);

	if (!zipdl.start(&err))
		return this->doShowErrAndClose(L"Error at download start", err);
	this->sendFunction([&]() {
		this->setText( String::Fmt(L"Downloading %s...", File::Path::GetFilename(this->dest)) );
	});

	if (!fout.setNewSize(zipdl.getContentLength(), &err))
		return this->doShowErrAndClose(L"Error when resizing file", err);

	return this->doReceiveData(zipdl, fout);
}

bool DlgDnZip::doReceiveData(Internet::Download& zipdl, File::Raw& fout)
{
	dbg(L"Response headers:\n");
	zipdl.getResponseHeaders().each([](const Hash<String>::Elem& rh) { // informational debug
		dbg(L"- %s: %s\n", rh.key.str(), rh.val.str());
	});

	String err;
	while (zipdl.hasData(&err)) {
		if (!fout.write(zipdl.getBuffer(), &err))
			return this->doShowErrAndClose(L"File writing error", err);

		this->sendFunction([&]() {
			this->label.setText( String::Fmt(L"%.0f%% downloaded (%.1f MB)...\n",
				zipdl.getPercent(), (float)zipdl.getTotalDownloaded() / 1024 / 1024 ));
			this->progBar.setPos((int)zipdl.getPercent());
		});
	}

	if (!err.isEmpty())
		return this->doShowErrAndClose(L"Download error", err);
	
	this->sendFunction([&]() { this->endDialog(IDOK); }); // download finished
	return true;
}

bool DlgDnZip::doShowErrAndClose(const wchar_t *msg, const String& err)
{
	this->sendFunction([&]() {
		this->messageBox(msg, err, MB_ICONERROR);
		this->endDialog(IDCANCEL);
	});
	return false;
}