
#include "Dlg_Dn_Dll.h"
#include <winlamb-more/file.h>
#include <winlamb-more/file_mapped.h>
#include <winlamb-more/path.h>
#include <winlamb-more/str.h>
#include <winlamb-more/sys.h>
#include <winlamb-more/zip.h>
#include "res/resource.h"
using namespace wl;
using std::vector;
using std::wstring;

Dlg_Dn_Dll::Dlg_Dn_Dll(progress_taskbar& tb, download::session& sess, const wstring& mk)
	: Dlg_Dn(tb), m_session(sess), m_marker(mk)
{
	on_message(WM_INITDIALOG, [&](params&)
	{
		init_controls();
		SetWindowText(hwnd(), L"Downloading chrome-win32.zip...");
		sys::start_thread([&]() {
			_download(); // start right away
		});
		return TRUE;
	});

	handle_close_msg();
}

bool Dlg_Dn_Dll::_download()
{
	wstring lnk = str::format(L"%s/%schrome-win32.zip",
		BASE_URL, m_marker.c_str() );

	download dlfile(m_session, lnk);
	dlfile.set_referrer(REFERRER);
	dlfile.add_request_header(L"Accept-Encoding", L"gzip,deflate,sdch")
		.add_request_header(L"Connection", L"keep-alive")
		.add_request_header(L"DNT", L"1")
		.add_request_header(L"Host", L"commondatastorage.googleapis.com");

	wstring err;
	wstring destPath = sys::get_exe_path().append(L"\\tmpchro.zip");
	file fout;
	if (!fout.open_or_create(destPath, &err)) {
		return show_err_and_close(L"File creation error", err);
	}

	dlfile.on_start([&]() {
		if (!fout.set_new_size(dlfile.get_content_length(), &err)) {
			return show_err_and_close(L"Error when resizing file", err);
		}
		return true;
	});

	dlfile.on_progress([&]() {
		if (!fout.write(dlfile.data, &err)) {
			return show_err_and_close(L"File writing error", err);
		}
		dlfile.data.clear(); // flushing to file right away, so clear download buffer

		run_ui_thread([&]() {
			m_progBar.set_pos(dlfile.get_percent());
			m_taskbarProg.set_pos(dlfile.get_percent());
			m_lblTitle.set_text( str::format(L"%.0f%% downloaded (%.1f MB)...\n",
				dlfile.get_percent(),
				static_cast<float>(dlfile.get_total_downloaded()) / 1024 / 1024) );
		});
		return true;
	});

	if (!dlfile.start(&err)) {
		return show_err_and_close(L"Download error", err);
	}
	
	fout.close();
	return _read_version(destPath);
}

bool Dlg_Dn_Dll::_read_version(wstring zipPath)
{
	// Unzip the package.
	run_ui_thread([&]() {
		SetWindowText(hwnd(), L"Processing package...");
		m_lblTitle.set_text(L"Unzipping chrome.dll, please wait...");
		m_progBar.set_waiting(true);
		m_taskbarProg.set_waiting(true);
	});
	wstring err;
	if (!zip::extract_all(zipPath, path::folder_from(zipPath), &err)) { // potentially slow
		return show_err_and_close(L"Unzipping failed", err);
	}

	// Open chrome.dll as memory-mapped.
	run_ui_thread([&]() {
		m_lblTitle.set_text(L"Scanning chrome.dll, please wait...");
	});
	wstring dllPath = path::folder_from(zipPath).append(L"\\chrome-win32\\chrome.dll");
	if (!file::exists(dllPath)) {
		return show_err_and_close(L"DLL not found",
			str::format(L"Could not find DLL:\n%s\n%s", dllPath.c_str()) );
	}

	file_mapped file;
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
	version.assign(reinterpret_cast<const wchar_t*>(pData + startAt + match2 + 30), 11);
	
	file.close();
	file::del(path::folder_from(zipPath).append(L"\\chrome-win32"));
	file::del(zipPath);
		
	run_ui_thread([&]() {
		m_taskbarProg.clear();
		EndDialog(hwnd(), IDOK);
	});
	return true;
}

int Dlg_Dn_Dll::_find_in_binary(const BYTE *pData, size_t dataLen, const wchar_t *what, bool asWideChar)
{
	// Returns the position of a string within a binary data block, if present.

	size_t whatlen = lstrlen(what);
	size_t pWhatSz = whatlen * (asWideChar ? 2 : 1);

	vector<BYTE> whatBuffered;
	whatBuffered.resize(pWhatSz, 0);

	if (asWideChar) {
		memcpy(&whatBuffered[0], what, whatlen * sizeof(wchar_t)); // simply copy the wide string, each char+zero
	} else {
		for (size_t i = 0; i < whatlen; ++i) {
			whatBuffered[i] = LOBYTE(what[i]); // raw conversion from wchar_t to char
		}
	}

	for (size_t i = 0; i < dataLen - pWhatSz; ++i) {
		if (!memcmp(pData + i, &whatBuffered[0], pWhatSz * sizeof(BYTE))) {
			return static_cast<int>(i);
		}
	}
	return -1; // not found
}