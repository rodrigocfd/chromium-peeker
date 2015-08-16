
#include "DlgDnZip.h"
using namespace wolf;
using std::wstring;

void DlgDnZip::events()
{
	this->onMessage(WM_INITDIALOG, [&](WPARAM wp, LPARAM lp)->INT_PTR
	{
		DlgDn::_initCtrls();
		this->setText(L"Downloading chrome-win32.zip...");

		wstring defSave = sys::GetDesktopPath().append(L"\\chrome-win32.zip");
		if (this->getFileSave(L"Zip file (*.zip)|*.zip", _dest, defSave.c_str())) {
			sys::Thread([&]() {
				this->_doDownload(); // start right away
			});
		} else {
			this->endDialog(IDCANCEL);
		}
		return TRUE;
	});
}

bool DlgDnZip::_doDownload()
{
	wstring lnk = str::Sprintf(
		L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip",
		_marker.c_str() );

	net::Download zipdl(_session, lnk);
	zipdl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	zipdl.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err;
	file::Raw fout;
	if (!fout.open(_dest, file::Access::READWRITE, &err)) {
		return DlgDn::_doShowErrAndClose(L"File creation error", err);
	}

	if (!zipdl.start(&err)) {
		return DlgDn::_doShowErrAndClose(L"Error at download start", err);
	}
	this->inOrigThread([&]() {
		this->setText( str::Sprintf(L"Downloading %s...", file::path::GetFilename(_dest)) );
	});

	if (!fout.setNewSize(zipdl.getContentLength(), &err)) {
		return DlgDn::_doShowErrAndClose(L"Error when resizing file", err);
	}

	return this->_doReceiveData(zipdl, fout);
}

bool DlgDnZip::_doReceiveData(net::Download& zipdl, file::Raw& fout)
{
	/*dbg(L"Response headers:\n");
	zipdl.getResponseHeaders().each([](const Hash<String>::Elem& rh) { // informational debug
		dbg(L"- %s: %s\n", rh.key.str(), rh.val.str());
	});*/

	wstring err;
	while (zipdl.hasData(&err)) {
		if (!fout.write(zipdl.getBuffer(), &err)) {
			return DlgDn::_doShowErrAndClose(L"File writing error", err);
		}
		this->inOrigThread([&]() {
			_label.setText( str::Sprintf(L"%.0f%% downloaded (%.1f MB)...\n",
				zipdl.getPercent(), static_cast<float>(zipdl.getTotalDownloaded()) / 1024 / 1024 ));
			_progBar.setPos(static_cast<int>(zipdl.getPercent()));
		});
	}

	if (!err.empty()) {
		return DlgDn::_doShowErrAndClose(L"Download error", err);
	}
	this->inOrigThread([&]() {
		this->endDialog(IDOK); // download finished
	});
	return true;
}