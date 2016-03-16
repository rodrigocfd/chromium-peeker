
#include "FrmDnZip.h"
#include "../winutil/Path.h"
#include "../winutil/Str.h"
#include "../winutil/Sys.h"
using std::wstring;

FrmDnZip::FrmDnZip(TaskBarProgress& taskBar,
	InternetSession& session,
	const wstring& marker)
	: FrmDn(taskBar), _session(session), _marker(marker)
{
	on_message(WM_INITDIALOG, [this](params p)->INT_PTR
	{
		initControls();
		SetWindowText(hwnd(), L"Downloading chrome-win32.zip...");

		wstring defSave = Sys::pathOfDesktop().append(L"\\chrome-win32.zip");
		if (File::showSave(hwnd(), L"Zip file (*.zip)|*.zip", _dest, defSave.c_str())) {
			Sys::thread([this]() {
				_doDownload(); // start right away
			});
		} else {
			EndDialog(hwnd(), IDCANCEL);
		}
		return TRUE;
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
		return doShowErrAndClose(L"File creation error", err);
	}

	if (!zipdl.start(&err)) {
		return doShowErrAndClose(L"Error at download start", err);
	}
	ui_thread([this]()->void {
		SetWindowText(hwnd(), Str::format(L"Downloading %s...",
			Path::fileFrom(_dest).c_str()).c_str() );
	});

	if (!fout.setNewSize(zipdl.getContentLength(), &err)) {
		return doShowErrAndClose(L"Error when resizing file", err);
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
			return doShowErrAndClose(L"File writing error", err);
		}
		ui_thread([&]()->void {
			_label.setText( Str::format(L"%.0f%% downloaded (%.1f MB)...\n",
				zipdl.getPercent(), static_cast<float>(zipdl.getTotalDownloaded()) / 1024 / 1024 ) );
			_progBar.setPos(static_cast<int>(zipdl.getPercent()));
		});
	}

	if (!err.empty()) {
		return doShowErrAndClose(L"Download error", err);
	}
	ui_thread([this]()->void {
		EndDialog(hwnd(), IDOK); // download finished
	});
	return true;
}