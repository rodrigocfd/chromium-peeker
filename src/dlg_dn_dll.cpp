
#include "dlg_dn_dll.h"
#include "../winutil/file.h"
#include "../winutil/file_map.h"
#include "../winutil/path.h"
#include "../winutil/str.h"
#include "../winutil/sys.h"
#include "../res/resource.h"
using namespace winutil;
using std::wstring;

dlg_dn_dll::dlg_dn_dll(taskbar_progress& taskBar,
	internet_session& session,
	const wstring& marker)
	: dlg_dn(taskBar), _session(session), _marker(marker), _totDownloaded(0)
{
	on.INITDIALOG([this](par::initdialog p)->INT_PTR
	{
		init_controls();
		SetWindowText(hwnd(), L"Downloading chrome-win32.zip...");
		sys::thread([this]()->void {
			_download(); // start right away
		});
		return TRUE;
	});
}

bool dlg_dn_dll::_download()
{
	wstring lnk = str::format(
		L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip",
		_marker.c_str() );

	internet_download dlfile(_session, lnk);
	dlfile.set_referrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dlfile.add_request_header({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err, destPath = sys::path_of_exe().append(L"\\tmpchro.zip");
	file fout;
	if (!fout.open(destPath, file::access::READWRITE, &err)) {
		return show_err_and_close(L"File creation error", err);
	}
	if (!dlfile.start(&err)) {
		return show_err_and_close(L"Error at download start", err);
	}
	if (!fout.set_new_size(dlfile.get_content_length(), &err)) {
		return show_err_and_close(L"Error when resizing file", err);
	}
	while (dlfile.has_data(&err)) {
		if (!fout.write(dlfile.get_buffer(), &err)) {
			return show_err_and_close(L"File writing error", err);
		}
		on_ui_thread([&]()->void {
			_progBar.set_pos(dlfile.get_percent());
			_taskBar.set_pos(dlfile.get_percent());
			_label.set_text( str::format(L"%.0f%% downloaded (%.1f MB)...\n",
				dlfile.get_percent(), static_cast<float>(dlfile.get_total_downloaded()) / 1024 / 1024) );
		});
	}

	fout.close();
	if (!err.empty()) {
		return show_err_and_close(L"Download error", err);
	}
	return _read_version(destPath);
}

bool dlg_dn_dll::_read_version(wstring zipPath)
{
	// Unzip the package.
	on_ui_thread([this]()->void {
		SetWindowText(hwnd(), L"Processing package...");
		_label.set_text(L"Unzipping chrome.dll, please wait...");
		_progBar.set_waiting(true);
		_taskBar.set_waiting(true);
	});
	wstring err;
	if (!file::unzip(zipPath, path::folder_from(zipPath), &err)) { // potentially slow
		return show_err_and_close(L"Unzipping failed", err);
	}

	// Open chrome.dll as memory-mapped.
	on_ui_thread([this]()->void {
		_label.set_text(L"Scanning chrome.dll, please wait...");
	});
	wstring dllPath = path::folder_from(zipPath).append(L"\\chrome-win32\\chrome.dll");
	if (!file::exists(dllPath)) {
		return show_err_and_close(L"DLL not found",
			str::format(L"Could not find DLL:\n%s\n%s", dllPath.c_str()) );
	}

	file_map file;
	if (!file.open(dllPath, file::access::READONLY, &err)) {
		return show_err_and_close(L"Could not open file", err);
	}

	// Search strings.
	const wchar_t *term = L"ProductVersion";
	int startAt = 26 * 1024 * 1024; // use an offset to search less

	BYTE *pData = file.p_mem();
	int match1 = _find_in_binary(pData + startAt, file.size() - startAt, term, true); // 1st occurrence
	if (match1 == -1) {
		return show_err_and_close(L"Parsing error",
			str::format(L"1st version offset could not be found in:\n%s", dllPath.c_str()) );
	}

	startAt += match1 + lstrlen(term) * sizeof(wchar_t);
	int match2 = _find_in_binary(pData + startAt, file.size() - startAt, term, true); // 2st occurrence
	if (match2 == -1) {
		return show_err_and_close(L"Parsing error",
			str::format(L"2st version offset could not be found in:\n%s", dllPath.c_str()) );
	}
	this->version.assign(reinterpret_cast<const wchar_t*>(pData + startAt + match2 + 30), 11);
	
	file.close();
	file::del(path::folder_from(zipPath).append(L"\\chrome-win32"));
	file::del(zipPath);
		
	on_ui_thread([this]()->void {
		_taskBar.clear();
		EndDialog(hwnd(), IDOK);
	});
	return true;
}

int dlg_dn_dll::_find_in_binary(const BYTE *pData, size_t dataLen, const wchar_t *what, bool asWideChar)
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