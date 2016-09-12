
#include "dlg_dn_zip.h"
#include "../winutil/path.h"
#include "../winutil/str.h"
#include "../winutil/sys.h"
using namespace winutil;
using std::wstring;

dlg_dn_zip::dlg_dn_zip(taskbar_progress& taskBar,
	internet_session& session,
	const wstring& marker)
	: dlg_dn(taskBar), _session(session), _marker(marker)
{
	on.INITDIALOG([this](par::initdialog p)->INT_PTR
	{
		init_controls();
		SetWindowText(hwnd(), L"Downloading chrome-win32.zip...");

		wstring defSave = sys::path_of_desktop().append(L"\\chrome-win32.zip");
		if (sys::show_save_file(hwnd(), L"Zip file (*.zip)|*.zip", _dest, defSave.c_str())) {
			sys::thread([this]()->void {
				_download(); // start right away
			});
		} else {
			EndDialog(hwnd(), IDCANCEL);
		}
		return TRUE;
	});
}

bool dlg_dn_zip::_download()
{
	wstring lnk = str::format(
		L"http://commondatastorage.googleapis.com/chromium-browser-continuous/%schrome-win32.zip",
		_marker.c_str() );

	internet_download zipdl(_session, lnk);
	zipdl.set_referrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	zipdl.add_request_header({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err;
	file fout;
	if (!fout.open(_dest, file::access::READWRITE, &err)) {
		return show_err_and_close(L"File creation error", err);
	}

	if (!zipdl.start(&err)) {
		return show_err_and_close(L"Error at download start", err);
	}
	on_ui_thread([this]()->void {
		SetWindowText(hwnd(), str::format(L"Downloading %s...",
			path::file_from(_dest).c_str()).c_str() );
	});

	if (!fout.set_new_size(zipdl.get_content_length(), &err)) {
		return show_err_and_close(L"Error when resizing file", err);
	}

	return _receive_data(zipdl, fout);
}

bool dlg_dn_zip::_receive_data(internet_download& zipdl, file& fout)
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
		on_ui_thread([&]()->void {
			_label.set_text( str::format(L"%.0f%% downloaded (%.1f MB)...\n",
				zipdl.get_percent(), static_cast<float>(zipdl.get_total_downloaded()) / 1024 / 1024 ) );
			_progBar.set_pos(static_cast<int>(zipdl.get_percent()));
		});
	}

	if (!err.empty()) {
		return show_err_and_close(L"Download error", err);
	}
	on_ui_thread([this]()->void {
		EndDialog(hwnd(), IDOK); // download finished
	});
	return true;
}