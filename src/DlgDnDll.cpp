
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
	switch(msg)
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

	this->doDownload();
}

void DlgDnDll::doDownload()
{
	Internet::Download dlfile(*this->pSession, this);
	dlfile.setDestFile(System::GetExePath().append(L"\\tmpchro.zip"));
	dlfile.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});
	dlfile.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");

	String lnk = String::Fmt(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip",
		this->marker.str() );

	dlfile.download(lnk, [&](Internet::Msg msgType, const Internet::Status *s) {
		switch(msgType) {
		case Internet::Msg::FAIL:
			this->messageBox(L"Download error", String::Fmt(L"Download failed.\n%s", s->err.str()), MB_ICONERROR);
			this->endDialog(IDCANCEL);
			break;
		/*case Internet::Msg::BEGIN:
			s->responseHeaders.each([](const Hash<String>::Elem& elem) {
				dbg(L"[%s] [%s]\n", elem.key.str(), elem.val.str() );
			});
			break;*/
		case Internet::Msg::PROGRESS:
			this->label.setText( String::Fmt(L"%.0f%% downloaded (%.1f MB)...\n",
				s->pct(), (float)s->totalDownloaded / 1024 / 1024 ));
			this->progBar.setPos((int)s->pct());
			break;
		case Internet::Msg::END:
			this->setText(L"Processing package...");
			this->label.setText(L"Unzipping and scanning chrome.dll, please wait...");
			//this->endDialog(this->doReadVersion(s->destPath) ? IDOK : IDCANCEL);
			this->doReadVersion(s->destPath);
		}
	});
}

void DlgDnDll::doReadVersion(String zipPath)
{
	System::Thread([=]() { // the zip is quite large, so the unzipping operation is potentially long, we use a thread
		auto showErrAndClose = [&](String errMsg) {
			this->sendFunction([=]() { // go back to window thread
				this->messageBox(L"Fail", errMsg.str(), MB_ICONERROR);
				this->endDialog(IDCANCEL);
			});
		};

		// Unzip the package.
		String err;
		if(!File::Unzip(zipPath.str(), File::Path::GetPath(zipPath).str(), &err)) {
			showErrAndClose( String::Fmt(L"Could not unzip package:\n%s\n%s", zipPath.str(), err.str()) );
			return;
		}

		// Open chrome.dll as memory-mapped.
		String dllPath = File::Path::GetPath(zipPath).append(L"\\chrome-win32\\chrome.dll");
		if(!File::Exists(dllPath)) {
			showErrAndClose( String::Fmt(L"Could not find DLL:\n%s\n%s", dllPath.str()) );
			return;
		}

		File::Mapped file;
		if(!file.open(dllPath, File::Access::READONLY, &err)) {
			showErrAndClose( String::Fmt(L"Could not open file:\n%s\n%s", dllPath.str(), err.str()) );
			return;
		}

		// Search strings.
		const wchar_t *term = L"ProductVersion";
		int startAt = 26 * 1024 * 1024; // use an offset to search less

		BYTE *pData = file.pMem();
		int match1 = File::IndexOfBin(pData + startAt, file.size() - startAt, term, true); // 1st occurrence
		if(match1 == -1) {
			showErrAndClose( String::Fmt(L"1st version offset could not be found in:\n%s", dllPath.str()) );
			return;
		}

		startAt += match1 + lstrlen(term) * sizeof(wchar_t);
		int match2 = File::IndexOfBin(pData + startAt, file.size() - startAt, term, true); // 2st occurrence
		if(match2 == -1) {
			showErrAndClose( String::Fmt(L"2st version offset could not be found in:\n%s", dllPath.str()) );
			return;
		}

		this->version.copyFrom((const wchar_t*)(pData + startAt + match2 + 30), 11);
	
		file.close();
		File::Delete(File::Path::GetPath(zipPath).append(L"\\chrome-win32"));
		File::Delete(zipPath);
		
		this->sendFunction([&]() { this->endDialog(IDOK); }); // all good
	});
}