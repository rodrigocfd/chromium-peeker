
#include "FrmDnDll.h"
#include "../res/resource.h"
using namespace wolf;
using std::wstring;

FrmDnDll::FrmDnDll(WindowMain *wmain, InternetSession& session, const wstring& marker)
	: FrmDn(wmain), _session(session), _marker(marker), _totDownloaded(0)
{
	this->onMessage(WM_CREATE, [this](WPARAM wp, LPARAM lp)->LRESULT
	{
		this->setText(L"Downloading chrome-win32.zip...");
		Sys::thread([this]() {
			this->_doDownload(); // start right away
		});
		return 0;
	});
}

bool FrmDnDll::_doDownload()
{
	wstring lnk = Str::format(
		L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip",
		_marker.c_str() );

	InternetDownload dlfile(_session, lnk);
	dlfile.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dlfile.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err, destPath = Sys::pathOfExe().append(L"\\tmpchro.zip");
	File fout;
	if (!fout.open(destPath, File::Access::READWRITE, &err)) {
		return _doShowErrAndClose(L"File creation error", err);
	}
	if (!dlfile.start(&err)) {
		return _doShowErrAndClose(L"Error at download start", err);
	}
	if (!fout.setNewSize(dlfile.getContentLength(), &err)) {
		return _doShowErrAndClose(L"Error when resizing file", err);
	}
	while (dlfile.hasData(&err)) {
		if (!fout.write(dlfile.getBuffer(), &err)) {
			return _doShowErrAndClose(L"File writing error", err);
		}
		this->guiThread([&]()->void {
			_progBar.setPos(dlfile.getPercent());
			_taskBar.setPos(dlfile.getPercent());
			_label.setText( Str::format(L"%.0f%% downloaded (%.1f MB)...\n",
				dlfile.getPercent(), static_cast<float>(dlfile.getTotalDownloaded()) / 1024 / 1024) );
		});
	}

	fout.close();
	if (!err.empty()) {
		return _doShowErrAndClose(L"Download error", err);
	}
	return _doReadVersion(destPath);
}

bool FrmDnDll::_doReadVersion(wstring zipPath)
{
	// Unzip the package.
	this->guiThread([this]()->void {
		this->setText(L"Processing package...");
		_label.setText(L"Unzipping chrome.dll, please wait...");
		_progBar.setWaiting(true);
		_taskBar.setWaiting(true);
	});
	wstring err;
	if (!File::unzip(zipPath, Str::folderFromPath(zipPath), &err)) { // potentially slow
		return _doShowErrAndClose(L"Unzipping failed", err);
	}

	// Open chrome.dll as memory-mapped.
	this->guiThread([this]()->void {
		_label.setText(L"Scanning chrome.dll, please wait...");
	});
	wstring dllPath = Str::folderFromPath(zipPath).append(L"\\chrome-win32\\chrome.dll");
	if (!File::exists(dllPath)) {
		return _doShowErrAndClose(L"DLL not found",
			Str::format(L"Could not find DLL:\n%s\n%s", dllPath.c_str()) );
	}

	FileMap file;
	if (!file.open(dllPath, File::Access::READONLY, &err)) {
		return _doShowErrAndClose(L"Could not open file", err);
	}

	// Search strings.
	const wchar_t *term = L"ProductVersion";
	int startAt = 26 * 1024 * 1024; // use an offset to search less

	BYTE *pData = file.pMem();
	int match1 = _findInBinary(pData + startAt, file.size() - startAt, term, true); // 1st occurrence
	if (match1 == -1) {
		return _doShowErrAndClose(L"Parsing error",
			Str::format(L"1st version offset could not be found in:\n%s", dllPath.c_str()) );
	}

	startAt += match1 + lstrlen(term) * sizeof(wchar_t);
	int match2 = _findInBinary(pData + startAt, file.size() - startAt, term, true); // 2st occurrence
	if (match2 == -1) {
		return _doShowErrAndClose(L"Parsing error",
			Str::format(L"2st version offset could not be found in:\n%s", dllPath.c_str()) );
	}
	this->version.assign((const wchar_t*)(pData + startAt + match2 + 30), 11);
	
	file.close();
	File::del(Str::folderFromPath(zipPath).append(L"\\chrome-win32"));
	File::del(zipPath);
		
	this->guiThread([this]()->void {
		_taskBar.dismiss();
		this->sendMessage(WM_CLOSE, 0, 0);
	});
	return true;
}

int FrmDnDll::_findInBinary(const BYTE *pData, size_t dataLen, const wchar_t *what, bool asWideChar)
{
	// Returns the position of a string within a binary data block, if present.

	size_t whatlen = lstrlen(what);
	size_t pWhatSz = whatlen * (asWideChar ? 2 : 1);
	BYTE *pWhat = static_cast<BYTE*>(_alloca(pWhatSz * sizeof(BYTE)));
	if (asWideChar) {
		memcpy(pWhat, what, whatlen * sizeof(wchar_t)); // simply copy the wide string, each char+zero
	} else {
		for (size_t i = 0; i < whatlen; ++i) {
			pWhat[i] = LOBYTE(what[i]); // raw conversion from wchar_t to char
		}
	}

	for (size_t i = 0; i < dataLen; ++i) {
		if (!memcmp(pData + i, pWhat, pWhatSz * sizeof(BYTE))) {
			return static_cast<int>(i);
		}
	}
	return -1; // not found
}