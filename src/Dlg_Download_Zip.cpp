
#include "Dlg_Download_Zip.h"
#include <winlamb/path.h>
#include <winlamb/str.h>
#include <winlamb/sysdlg.h>
#include <winlamb/syspath.h>
using std::wstring;
using namespace wl;

Dlg_Download_Zip::Dlg_Download_Zip(progress_taskbar& tb, download::session& sess,
	const wstring& mk) :
	Dlg_Download(tb), m_session(sess), m_marker(mk)
{
	on_message(WM_INITDIALOG, [&](params)
	{
		init_controls();
		set_text(L"Downloading chrome-win32.zip...");

		wstring defSave = syspath::desktop().append(L"\\chrome-win32.zip");
		if (sysdlg::save_file(this, L"Zip file (*.zip)|*.zip", m_dest, defSave)) {
			run_thread_detached([&]() {
				_download(); // start right away
			});
		} else {
			EndDialog(hwnd(), IDCANCEL);
		}
		return TRUE;
	});

	handle_close_msg();
}

void Dlg_Download_Zip::_download()
{
	wstring lnk = str::format(L"%s/%schrome-win32.zip", BASE_URL, m_marker);

	download zipdl(m_session, lnk);
	zipdl.set_referrer(REFERRER);
	zipdl.add_request_header(L"Accept-Encoding", L"gzip,deflate,sdch")
		.add_request_header(L"Connection", L"keep-alive")
		.add_request_header(L"DNT", L"1")
		.add_request_header(L"Host", L"commondatastorage.googleapis.com");

	file fout;
	try {
		fout.open_or_create(m_dest);
	} catch (const std::exception& e) {
		show_err_and_close(L"File creation error", str::to_wstring(e.what()));
		return;
	}

	zipdl.on_start([&]() {
		try {
			fout.set_new_size(zipdl.get_content_length());
		} catch (const std::exception& e) {
			zipdl.abort();
			fout.close();
			file::util::del(m_dest);
			show_err_and_close(L"Error when resizing file", str::to_wstring(e.what()));
			return;
		}

		run_thread_ui([&]() {
			set_text( str::format(L"Downloading %s...", path::file_from(m_dest)) );
		});
	});

	zipdl.on_progress([&]() {
		try {
			fout.write(zipdl.data);
		} catch (const std::exception& e) {
			zipdl.abort();
			fout.close();
			file::util::del(m_dest);
			show_err_and_close(L"File writing error", str::to_wstring(e.what()));
			return;
		}

		zipdl.data.clear(); // flushing to file right away, so clear download buffer

		run_thread_ui([&]() {
			m_lblTitle.set_text( str::format(L"%.0f%% downloaded (%.1f MB)...\n",
				zipdl.get_percent(),
				static_cast<float>(zipdl.get_total_downloaded()) / 1024 / 1024) );
			m_progBar.set_pos(static_cast<int>(zipdl.get_percent()));
		});
	});

	try {
		zipdl.start();
	} catch (const std::exception& e) {
		show_err_and_close(L"Download error", str::to_wstring(e.what()));
		return;
	}

	run_thread_ui([&]() {
		EndDialog(hwnd(), IDOK); // download finished
	});
}