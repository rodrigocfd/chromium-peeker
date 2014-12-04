
#include "DlgDnDll.h"
#include "../res/resource.h"

int DlgDnDll::show(Window *parent, Internet::Session *session, const String& marker)
{
	this->pSession = session;
	this->marker = marker;
	this->totDownloaded = 0;
	return DialogModal::show(parent, DLG_PROGRESS);
}

INT_PTR DlgDnDll::msgHandler(UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_INITDIALOG: onInitDialog(); break;
	}
	return DialogModal::msgHandler(msg, wp, lp);
}

void DlgDnDll::onInitDialog()
{
	this->setXButton(false);
	this->setText(L"Downloading chrome-win32.zip...");

	( this->label = this->getChild(LBL_LBL) )
		.setText(L"Waiting...");

	( this->progBar = this->getChild(PRO_PRO) )
		.setRange(0, 100)
		.setPos(0);

	System::Thread([&]() {
		this->doDownload();
	});
}

bool DlgDnDll::doDownload()
{
	String lnk = String::Fmt(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip",
		this->marker.str() );

	Internet::Download dlfile(*this->pSession, lnk);
	dlfile.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dlfile.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	String destPath = System::GetExePath().append(L"\\tmpchro.zip");
	String err;
	File::Raw fout;
	if (!fout.open(destPath, File::Access::READWRITE, &err))
		return this->doShowErrAndClose(L"File creation error", err);
	
	if (!dlfile.start(&err))
		return this->doShowErrAndClose(L"Error at download start", err);

	if (!fout.setNewSize(dlfile.getContentLength(), &err))
		return this->doShowErrAndClose(L"Error when resizing file", err);
		
	while (dlfile.hasData(&err)) {
		if (!fout.write(dlfile.getBuffer(), &err))
			return this->doShowErrAndClose(L"File writing error", err);

		this->sendFunction([&]() {
			this->progBar.setPos((int)dlfile.getPercent());
			this->label.setText( String::Fmt(L"%.0f%% downloaded (%.1f MB)...\n",
				dlfile.getPercent(), (float)dlfile.getTotalDownloaded() / 1024 / 1024 ));
		});
	}

	fout.close();
	if (!err.isEmpty())
		return this->doShowErrAndClose(L"Download error", err);

	return this->doReadVersion(destPath);
}

bool DlgDnDll::doReadVersion(String zipPath)
{
	// Unzip the package.
	this->sendFunction([&]() {
		this->setText(L"Processing package...");
		this->label.setText(L"Unzipping chrome.dll, please wait...");
		this->progBar.animateMarquee(true);
	});
	String err;
	if (!File::Unzip(zipPath, File::Path::GetPath(zipPath), &err)) // potentially slow
		return this->doShowErrAndClose(L"Unzipping failed", err);

	// Open chrome.dll as memory-mapped.
	this->sendFunction([&]() {
		this->label.setText(L"Scanning chrome.dll, please wait...");
	});
	String dllPath = File::Path::GetPath(zipPath).append(L"\\chrome-win32\\chrome.dll");
	if (!File::Exists(dllPath))
		return this->doShowErrAndClose(L"DLL not found",
			String::Fmt(L"Could not find DLL:\n%s\n%s", dllPath.str()) );

	File::Mapped file;
	if (!file.open(dllPath, File::Access::READONLY, &err))
		return this->doShowErrAndClose(L"Could not open file", err);

	// Search strings.
	const wchar_t *term = L"ProductVersion";
	int startAt = 26 * 1024 * 1024; // use an offset to search less

	BYTE *pData = file.pMem();
	int match1 = File::IndexOfBin(pData + startAt, file.size() - startAt, term, true); // 1st occurrence
	if (match1 == -1)
		return this->doShowErrAndClose(L"Parsing error",
			String::Fmt(L"1st version offset could not be found in:\n%s", dllPath.str()) );

	startAt += match1 + lstrlen(term) * sizeof(wchar_t);
	int match2 = File::IndexOfBin(pData + startAt, file.size() - startAt, term, true); // 2st occurrence
	if (match2 == -1)
		return this->doShowErrAndClose(L"Parsing error",
			String::Fmt(L"2st version offset could not be found in:\n%s", dllPath.str()) );

	this->version.copyFrom((const wchar_t*)(pData + startAt + match2 + 30), 11);
	
	file.close();
	File::Delete(File::Path::GetPath(zipPath).append(L"\\chrome-win32"));
	File::Delete(zipPath);
		
	this->sendFunction([&]() { this->endDialog(IDOK); });
	return true;
}

bool DlgDnDll::doShowErrAndClose(const wchar_t *msg, const String& err)
{
	this->sendFunction([&]() {
		this->messageBox(msg, err, MB_ICONERROR);
		this->endDialog(IDCANCEL);
	});
	return false;
}