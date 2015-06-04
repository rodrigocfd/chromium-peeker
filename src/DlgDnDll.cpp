
#include "DlgDnDll.h"
#include "../res/resource.h"
using namespace wolf;
using std::wstring;

void DlgDnDll::events()
{
	this->onMessage(WM_INITDIALOG, [&](WPARAM wp, LPARAM lp)->INT_PTR {
		DlgDn::_initCtrls();
		this->setText(L"Downloading chrome-win32.zip...");
		sys::Thread([&]() {
			this->_doDownload(); // start right away
		});
		return TRUE;
	});
}

bool DlgDnDll::_doDownload()
{
	wstring lnk = str::Sprintf(
		L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip",
		_marker.c_str() );

	net::Download dlfile(_session, lnk);
	dlfile.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dlfile.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err, destPath = sys::GetExePath().append(L"\\tmpchro.zip");
	file::Raw fout;
	if (!fout.open(destPath, file::Access::READWRITE, &err)) {
		return DlgDn::_doShowErrAndClose(L"File creation error", err);
	}
	if (!dlfile.start(&err)) {
		return DlgDn::_doShowErrAndClose(L"Error at download start", err);
	}
	if (!fout.setNewSize(dlfile.getContentLength(), &err)) {
		return DlgDn::_doShowErrAndClose(L"Error when resizing file", err);
	}
	while (dlfile.hasData(&err)) {
		if (!fout.write(dlfile.getBuffer(), &err)) {
			return DlgDn::_doShowErrAndClose(L"File writing error", err);
		}
		this->inOrigThread([&]() {
			_progBar.setPos(static_cast<int>(dlfile.getPercent()));
			_label.setText( str::Sprintf(L"%.0f%% downloaded (%.1f MB)...\n",
				dlfile.getPercent(), static_cast<float>(dlfile.getTotalDownloaded()) / 1024 / 1024 ));
		});
	}

	fout.close();
	if (!err.empty()) {
		return DlgDn::_doShowErrAndClose(L"Download error", err);
	}
	return this->_doReadVersion(destPath);
}

bool DlgDnDll::_doReadVersion(wstring zipPath)
{
	// Unzip the package.
	this->inOrigThread([&]() {
		this->setText(L"Processing package...");
		_label.setText(L"Unzipping chrome.dll, please wait...");
		_progBar.animateMarquee(true);
	});
	wstring err;
	if (!file::Unzip(zipPath, file::path::GetPath(zipPath), &err)) { // potentially slow
		return DlgDn::_doShowErrAndClose(L"Unzipping failed", err);
	}

	// Open chrome.dll as memory-mapped.
	this->inOrigThread([&]() {
		_label.setText(L"Scanning chrome.dll, please wait...");
	});
	wstring dllPath = file::path::GetPath(zipPath).append(L"\\chrome-win32\\chrome.dll");
	if (!file::Exists(dllPath)) {
		return DlgDn::_doShowErrAndClose(L"DLL not found",
			str::Sprintf(L"Could not find DLL:\n%s\n%s", dllPath.c_str()) );
	}

	file::Mapped file;
	if (!file.open(dllPath, file::Access::READONLY, &err)) {
		return DlgDn::_doShowErrAndClose(L"Could not open file", err);
	}

	// Search strings.
	const wchar_t *term = L"ProductVersion";
	int startAt = 26 * 1024 * 1024; // use an offset to search less

	BYTE *pData = file.pMem();
	int match1 = file::IndexOfBin(pData + startAt, file.size() - startAt, term, true); // 1st occurrence
	if (match1 == -1) {
		return DlgDn::_doShowErrAndClose(L"Parsing error",
			str::Sprintf(L"1st version offset could not be found in:\n%s", dllPath.c_str()) );
	}

	startAt += match1 + lstrlen(term) * sizeof(wchar_t);
	int match2 = file::IndexOfBin(pData + startAt, file.size() - startAt, term, true); // 2st occurrence
	if (match2 == -1) {
		return DlgDn::_doShowErrAndClose(L"Parsing error",
			str::Sprintf(L"2st version offset could not be found in:\n%s", dllPath.c_str()) );
	}
	this->version.assign((const wchar_t*)(pData + startAt + match2 + 30), 11);
	
	file.close();
	file::Delete(file::path::GetPath(zipPath).append(L"\\chrome-win32"));
	file::Delete(zipPath);
		
	this->inOrigThread([&]() {
		this->endDialog(IDOK);
	});
	return true;
}