
#include "Dlg_Dn_Dll.h"
#include <winlamb/file.h>
#include <winlamb/file_mapped.h>
#include <winlamb/path.h>
#include <winlamb/thread.h>
#include <winlamb/str.h>
#include <winlamb/version.h>
#include <winlamb/zip.h>
#include "res/resource.h"
using namespace wl;

Dlg_Dn_Dll::Dlg_Dn_Dll(progress_taskbar& tb, download::session& sess,
	const wstring& mk)
	: Dlg_Dn(tb), m_session(sess), m_marker(mk)
{
	on_message(WM_INITDIALOG, [&](wm::initdialog)
	{
		init_controls();
		set_text(L"Downloading chrome-win32.zip...");
		thread::run_detached([&]() {
			_download(); // start right away
		});
		return TRUE;
	});

	handle_close_msg();
}

void Dlg_Dn_Dll::_download()
{
	wstring lnk = str::format(L"%s/%schrome-win32.zip",
		BASE_URL, m_marker);

	download dnFile(m_session, lnk);
	dnFile.set_referrer(REFERRER);
	dnFile.add_request_header(L"Accept-Encoding", L"gzip,deflate,sdch")
		.add_request_header(L"Connection", L"keep-alive")
		.add_request_header(L"DNT", L"1")
		.add_request_header(L"Host", L"commondatastorage.googleapis.com");

	file fout;
	wstring destPath = path::get::exe_itself().append(L"\\tmpchro.zip");

	dnFile.on_start([&]() {
		try {
			fout.set_new_size(dnFile.get_content_length());
		} catch (const std::exception& e) {
			dnFile.abort();
			fout.close();
			file::del(destPath);
			show_err_and_close(L"Error when resizing file",
				str::parse_ascii(e.what()));
		}
	});

	dnFile.on_progress([&]() {
		try {
			fout.write(dnFile.data);
		} catch (const std::exception& e) {
			dnFile.abort();
			fout.close();
			file::del(destPath);
			show_err_and_close(L"File writing error", str::parse_ascii(e.what()));
			return;
		}

		dnFile.data.clear(); // flushing to file right away, so clear download buffer
		run_ui_thread([&]() {
			m_progBar.set_pos(dnFile.get_percent());
			m_taskbarProg.set_pos(dnFile.get_percent());
			m_lblTitle.set_text( str::format(L"%.0f%% downloaded (%.1f MB)...\n",
				dnFile.get_percent(),
				static_cast<float>(dnFile.get_total_downloaded()) / 1024 / 1024) );
		});
	});

	try {
		fout.open_or_create(destPath);
		dnFile.start();
	} catch (const std::exception& e) {
		show_err_and_close(L"Download error", str::parse_ascii(e.what()));
		return;
	}

	fout.close();
	_read_version(destPath);
}

void Dlg_Dn_Dll::_read_version(wstring zipPath)
{
	// Unzip the package.
	run_ui_thread([&]() {
		set_text(L"Processing package...");
		m_lblTitle.set_text(L"Unzipping chrome.dll, please wait...");
		m_progBar.set_waiting(true);
		m_taskbarProg.set_waiting(true);
	});

	try {
		zip::extract_all(zipPath, path::folder_from(zipPath)); // potentially slow
	} catch (const std::exception& e) {
		show_err_and_close(L"Unzipping failed", str::parse_ascii(e.what()));
		return;
	}

	// Check chrome.dll file.
	run_ui_thread([&]() {
		m_lblTitle.set_text(L"Scanning chrome.dll, please wait...");
	});

	wstring dllPath = path::folder_from(zipPath).append(L"\\chrome-win32\\chrome.dll");
	if (!file::exists(dllPath)) {
		show_err_and_close(L"DLL not found",
			str::format(L"Could not find DLL:\n%s\n%s", dllPath));
		return;
	}

	// Read chrome.dll version.
	version dllVer;
	try {
		if (!dllVer.read(dllPath)) {
			show_err_and_close(L"Version reading failed",
				L"No version info found on DLL.");
			return;
		}
	} catch (const std::exception& e) {
		show_err_and_close(L"Version reading failed",
			str::parse_ascii(e.what()));
		return;
	}

	versionNo = dllVer.to_string();

	// Cleanup.
	file::del(path::folder_from(zipPath).append(L"\\chrome-win32"));
	file::del(zipPath);

	run_ui_thread([&]() {
		m_taskbarProg.clear();
		EndDialog(hwnd(), IDOK);
	});
}