
#include "Dlg_Dn_Zip.h"
#include <winlamb-more/path.h>
#include <winlamb-more/str.h>
#include <winlamb-more/sys.h>
#include <winlamb-more/sysdlg.h>
using namespace wl;
using std::wstring;

Dlg_Dn_Zip::Dlg_Dn_Zip(progress_taskbar& tb, download::session& sess, const wstring& mk)
	: Dlg_Dn(tb), m_session(sess), m_marker(mk)
{
	on_message(WM_INITDIALOG, [&](params&)
	{
		init_controls();
		SetWindowText(hwnd(), L"Downloading chrome-win32.zip...");

		wstring defSave = sys::get_desktop_path().append(L"\\chrome-win32.zip");
		if (sysdlg::save_file(this, L"Zip file (*.zip)|*.zip", m_dest, defSave.c_str())) {
			sys::start_thread([&]() {
				_download(); // start right away
			});
		} else {
			EndDialog(hwnd(), IDCANCEL);
		}
		return TRUE;
	});

	handle_close_msg();
}

bool Dlg_Dn_Zip::_download()
{
	wstring lnk = str::format(L"%s/%schrome-win32.zip",
		BASE_URL, m_marker.c_str() );

	download zipdl(m_session, lnk);
	zipdl.set_referrer(REFERRER);
	zipdl.add_request_header(L"Accept-Encoding", L"gzip,deflate,sdch")
		.add_request_header(L"Connection", L"keep-alive")
		.add_request_header(L"DNT", L"1")
		.add_request_header(L"Host", L"commondatastorage.googleapis.com");

	wstring err;
	file fout;
	if (!fout.open_or_create(m_dest, &err)) {
		return show_err_and_close(L"File creation error", err);
	}
	
	zipdl.on_start([&]() {
		if (!fout.set_new_size(zipdl.get_content_length(), &err)) {
			zipdl.abort();
			fout.close();
			file::del(m_dest);
			show_err_and_close(L"Error when resizing file", err);
		} else {
			run_ui_thread([&]() {
				SetWindowText(hwnd(), str::format(L"Downloading %s...",
					path::file_from(m_dest).c_str()).c_str() );
			});
		}
	});

	zipdl.on_progress([&]() {
		if (!fout.write(zipdl.data, &err)) {
			zipdl.abort();
			fout.close();
			file::del(m_dest);
			show_err_and_close(L"File writing error", err);
		} else {
			zipdl.data.clear(); // flushing to file right away, so clear download buffer
			run_ui_thread([&]() {
				m_lblTitle.set_text( str::format(L"%.0f%% downloaded (%.1f MB)...\n",
					zipdl.get_percent(),
					static_cast<float>(zipdl.get_total_downloaded()) / 1024 / 1024 ) );
				m_progBar.set_pos(static_cast<int>(zipdl.get_percent()));
			});
		}
	});

	if (!zipdl.start(&err)) {
		return show_err_and_close(L"Download error", err);
	}

	run_ui_thread([&]() {
		EndDialog(hwnd(), IDOK); // download finished
	});
	return true;
}