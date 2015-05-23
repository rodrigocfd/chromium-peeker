
#include "DlgDnZip.h"
#include "../res/resource.h"

void DlgDnZip::onInitDialog()
{
	DlgDn::initCtrls();
	this->setText(L"Downloading chrome-win32.zip...");

	wstring defSave = sys::GetDesktopPath().append(L"\\chrome-win32.zip");
	if (this->getFileSave(L"Zip file (*.zip)|*.zip", this->dest, defSave.c_str())) {
		sys::Thread([&]() {
			this->doDownload(); // start right away
		});
	} else {
		this->endDialog(IDCANCEL);
	}
}

bool DlgDnZip::doDownload()
{
	wstring lnk = str::Sprintf(
		L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip",
		this->marker.c_str() );

	net::Download zipdl(this->session, lnk);
	zipdl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	zipdl.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err;
	file::Raw fout;
	if (!fout.open(this->dest, file::Access::READWRITE, &err)) {
		return DlgDn::doShowErrAndClose(L"File creation error", err);
	}

	if (!zipdl.start(&err)) {
		return DlgDn::doShowErrAndClose(L"Error at download start", err);
	}
	this->sendFunction([&]() {
		this->setText( str::Sprintf(L"Downloading %s...", file::path::GetFilename(this->dest)) );
	});

	if (!fout.setNewSize(zipdl.getContentLength(), &err)) {
		return DlgDn::doShowErrAndClose(L"Error when resizing file", err);
	}

	return this->doReceiveData(zipdl, fout);
}

bool DlgDnZip::doReceiveData(net::Download& zipdl, file::Raw& fout)
{
	/*dbg(L"Response headers:\n");
	zipdl.getResponseHeaders().each([](const Hash<String>::Elem& rh) { // informational debug
		dbg(L"- %s: %s\n", rh.key.str(), rh.val.str());
	});*/

	wstring err;
	while (zipdl.hasData(&err)) {
		if (!fout.write(zipdl.getBuffer(), &err)) {
			return DlgDn::doShowErrAndClose(L"File writing error", err);
		}
		this->sendFunction([&]() {
			this->label.setText( str::Sprintf(L"%.0f%% downloaded (%.1f MB)...\n",
				zipdl.getPercent(), (float)zipdl.getTotalDownloaded() / 1024 / 1024 ));
			this->progBar.setPos((int)zipdl.getPercent());
		});
	}

	if (!err.empty()) {
		return DlgDn::doShowErrAndClose(L"Download error", err);
	}
	this->sendFunction([&]() { this->endDialog(IDOK); }); // download finished
	return true;
}