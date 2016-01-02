
#include "FrmDnZip.h"
using namespace wolf;
using std::wstring;

FrmDnZip::FrmDnZip(TaskBarProgress& taskBar,
	InternetSession& session,
	const wstring& marker)
	: FrmDn(taskBar), _session(session), _marker(marker)
{
	this->onMessage(WM_CREATE, [&](WPARAM wp, LPARAM lp)->LRESULT
	{
		this->setText(L"Downloading chrome-win32.zip...");

		wstring defSave = Sys::pathOfDesktop().append(L"\\chrome-win32.zip");
		if (File::showSave(this, L"Zip file (*.zip)|*.zip", _dest, defSave.c_str())) {
			Sys::thread([this]() {
				_doDownload(); // start right away
			});
		} else {
			this->sendMessage(WM_CLOSE, 0, 0); // RETURN VALUE !!!!!
		}
		return 0;
	});
}

bool FrmDnZip::_doDownload()
{
	wstring lnk = Str::format(
		L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip",
		_marker.c_str() );

	InternetDownload zipdl(_session, lnk);
	zipdl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	zipdl.addRequestHeader({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err;
	File fout;
	if (!fout.open(_dest, File::Access::READWRITE, &err)) {
		return _doShowErrAndClose(L"File creation error", err);
	}

	if (!zipdl.start(&err)) {
		return _doShowErrAndClose(L"Error at download start", err);
	}
	this->guiThread([this]()->void {
		this->setText( Str::format(L"Downloading %s...", Str::fileFromPath(_dest).c_str()) );
	});

	if (!fout.setNewSize(zipdl.getContentLength(), &err)) {
		return _doShowErrAndClose(L"Error when resizing file", err);
	}

	return _doReceiveData(zipdl, fout);
}

bool FrmDnZip::_doReceiveData(InternetDownload& zipdl, File& fout)
{
	/*dbg(L"Response headers:\n");
	zipdl.getResponseHeaders().each([](const Hash<String>::Elem& rh) { // informational debug
		dbg(L"- %s: %s\n", rh.key.str(), rh.val.str());
	});*/

	wstring err;
	while (zipdl.hasData(&err)) {
		if (!fout.write(zipdl.getBuffer(), &err)) {
			return _doShowErrAndClose(L"File writing error", err);
		}
		this->guiThread([&]()->void {
			_label.setText( Str::format(L"%.0f%% downloaded (%.1f MB)...\n",
				zipdl.getPercent(), static_cast<float>(zipdl.getTotalDownloaded()) / 1024 / 1024 ) );
			_progBar.setPos(static_cast<int>(zipdl.getPercent()));
		});
	}

	if (!err.empty()) {
		return _doShowErrAndClose(L"Download error", err);
	}
	this->guiThread([this]()->void {
		this->sendMessage(WM_CLOSE, 0, 0); // download finished
	});
	return true;
}