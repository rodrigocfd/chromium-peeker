
#include "Dlg_Dn_Zip.h"
#include "../winlamb/path.h"
#include "../winlamb/str.h"
#include "../winlamb/sys.h"
#include "../winlamb/sysdlg.h"
using namespace wl;
using std::wstring;

Dlg_Dn_Zip::Dlg_Dn_Zip(progress_taskbar& tb, download::session& sess, const wstring& mk)
	: Dlg_Dn(tb), m_session(sess), m_marker(mk)
{
	on_message(WM_INITDIALOG, [&](params&)
	{
		init_controls();
		set_text(L"Downloading chrome-win32.zip...");

		wstring defSave = path::desktop_path().append(L"\\chrome-win32.zip");
		if (sysdlg::save_file(this, L"Zip file (*.zip)|*.zip", m_dest, defSave.c_str())) {
			sys::thread([&]() {
				_download(); // start right away
			});
		} else {
			EndDialog(hwnd(), IDCANCEL);
		}
		return TRUE;
	});
}

bool Dlg_Dn_Zip::_download()
{
	wstring lnk = str::format(L"%s/%schrome-win32.zip",
		BASE_URL, m_marker.c_str() );

	download zipdl(m_session, lnk);
	zipdl.set_referrer(REFERRER);
	zipdl.add_request_header({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err;
	file fout;
	if (!fout.open_or_create(m_dest, &err)) {
		return show_err_and_close(L"File creation error", err);
	}

	if (!zipdl.start(&err)) {
		return show_err_and_close(L"Error at download start", err);
	}
	ui_thread([&]() {
		set_text(str::format(L"Downloading %s...",
			path::file_from(m_dest).c_str()).c_str() );
	});

	if (!fout.set_new_size(zipdl.get_content_length(), &err)) {
		return show_err_and_close(L"Error when resizing file", err);
	}

	return _receive_data(zipdl, fout);
}

bool Dlg_Dn_Zip::_receive_data(download& zipdl, file& fout)
{
	/*dbg(L"Response headers:\n");
	zipdl.getResponseHeaders().each([](const Hash<String>::Elem& rh) { // informational debug
		dbg(L"- %s: %s\n", rh.key.str(), rh.val.str());
	});*/

	wstring err;
	while (zipdl.has_data(&err)) {
		if (!fout.write(zipdl.get_buffer(), &err)) {
			return show_err_and_close(L"File writing error", err);
		}
		ui_thread([&]() {
			m_lblTitle.set_text( str::format(L"%.0f%% downloaded (%.1f MB)...\n",
				zipdl.get_percent(),
				static_cast<float>(zipdl.get_total_downloaded()) / 1024 / 1024 ) );
			m_progBar.set_pos(static_cast<int>(zipdl.get_percent()));
		});
	}

	if (!err.empty()) {
		return show_err_and_close(L"Download error", err);
	}
	ui_thread([&]() {
		EndDialog(hwnd(), IDOK); // download finished
	});
	return true;
}