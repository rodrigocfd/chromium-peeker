
#include "DlgDnDll.h"
#include "../res/resource.h"

DlgDnDll::DlgDnDll(Internet::Session& isess, const wstring& mark)
	: session(isess), marker(mark), totDownloaded(0)
{
}

void DlgDnDll::events()
{
	this->defineDialog(DLG_PROGRESS);

	this->onInitDialog([&]() {
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
	});
}

bool DlgDnDll::doDownload()
{
	wstring lnk = Sprintf(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip",
		this->marker.c_str() );

	Internet::Download dlfile(this->session, lnk);
	dlfile.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dlfile.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err, destPath = System::GetExePath().append(L"\\tmpchro.zip");
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
			this->label.setText( Sprintf(L"%.0f%% downloaded (%.1f MB)...\n",
				dlfile.getPercent(), (float)dlfile.getTotalDownloaded() / 1024 / 1024 ));
		});
	}

	fout.close();
	if (!err.empty())
		return this->doShowErrAndClose(L"Download error", err);

	return this->doReadVersion(destPath);
}

bool DlgDnDll::doReadVersion(wstring zipPath)
{
	// Unzip the package.
	this->sendFunction([&]() {
		this->setText(L"Processing package...");
		this->label.setText(L"Unzipping chrome.dll, please wait...");
		this->progBar.animateMarquee(true);
	});
	wstring err;
	if (!File::Unzip(zipPath, File::Path::GetPath(zipPath), &err)) // potentially slow
		return this->doShowErrAndClose(L"Unzipping failed", err);

	// Open chrome.dll as memory-mapped.
	this->sendFunction([&]() {
		this->label.setText(L"Scanning chrome.dll, please wait...");
	});
	wstring dllPath = File::Path::GetPath(zipPath).append(L"\\chrome-win32\\chrome.dll");
	if (!File::Exists(dllPath))
		return this->doShowErrAndClose(L"DLL not found",
			Sprintf(L"Could not find DLL:\n%s\n%s", dllPath.c_str()) );

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
			Sprintf(L"1st version offset could not be found in:\n%s", dllPath.c_str()) );

	startAt += match1 + lstrlen(term) * sizeof(wchar_t);
	int match2 = File::IndexOfBin(pData + startAt, file.size() - startAt, term, true); // 2st occurrence
	if (match2 == -1)
		return this->doShowErrAndClose(L"Parsing error",
			Sprintf(L"2st version offset could not be found in:\n%s", dllPath.c_str()) );

	this->version.assign((const wchar_t*)(pData + startAt + match2 + 30), 11);
	
	file.close();
	File::Delete(File::Path::GetPath(zipPath).append(L"\\chrome-win32"));
	File::Delete(zipPath);
		
	this->sendFunction([&]() { this->endDialog(IDOK); });
	return true;
}

bool DlgDnDll::doShowErrAndClose(const wchar_t *msg, const wstring& err)
{
	this->sendFunction([&]() {
		this->messageBox(msg, err, MB_ICONERROR);
		this->endDialog(IDCANCEL);
	});
	return false;
}