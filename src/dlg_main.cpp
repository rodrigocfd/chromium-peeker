
#include "dlg_main.h"
#include "dlg_dn_dll.h"
#include "dlg_dn_info.h"
#include "dlg_dn_list.h"
#include "dlg_dn_zip.h"
#include "../winutil/str.h"
#include "../winutil/sys.h"
#include "../res/resource.h"
using namespace winutil;
using std::vector;
using std::wstring;

RUN(dlg_main);

dlg_main::dlg_main()
{
	setup.dialogId = DLG_MAIN;
	setup.iconId = ICO_CHROMIUM;

	on_message(WM_INITDIALOG, [this](params p)->INT_PTR
	{
		_taskBar.init(hwnd());

		(_listview = GetDlgItem(hwnd(), LST_BUILDS))
			.set_full_row_select()
			.set_context_menu(MEN_MAIN)
			.column_add(L"Build marker", 80)
			.column_add(L"Release date", 105)
			.column_add(L"Zip size", 65)
			.column_add(L"DLL version", 90)
			.column_fit(3);

		_lblLoaded = GetDlgItem(hwnd(), LBL_LOADED);

		_resizer.add(hwnd(), LBL_LOADED, resizer::go::RESIZE, resizer::go::NOTHING)
			.add(hwnd(), LST_BUILDS, resizer::go::RESIZE, resizer::go::RESIZE);

		wstring err;
		if (!_session.init(&err)) { // initialize internet session, for the whole program running time
			sys::msg_box(hwnd(), L"Fail", err, MB_ICONERROR);
			SendMessage(hwnd(), WM_CLOSE, 0, 0);
		}

		sys::set_wheel_hover_behavior(hwnd());
		return TRUE;
	});

	on_message(WM_SIZE, [this](params p)->INT_PTR
	{
		_resizer.arrange(p.wParam, p.lParam);
		_listview.column_fit(3);
		return TRUE;
	});

	on_initmenupopup(MNU_MAIN_GETBASIC, [this](params_initmenupopup p)->INT_PTR
	{
		menu menu = p.hmenu();
		size_t numSelec = _listview.items.count_selected();
		menu.enable_item({ MNU_MAIN_GETBASIC, MNU_MAIN_GETDLL }, numSelec >= 1)
			.enable_item(MNU_MAIN_DLZIP, numSelec == 1);
		return TRUE;
	});

	on_command(BTN_DLLIST, [this](params_command p)->INT_PTR
	{
		label btnDlList = GetDlgItem(hwnd(), BTN_DLLIST);
		btnDlList.enable(false);
		_chromium_rel.reset();
		_listview.items.remove_all();
		_listview.column_fit(3);
		_lblLoaded.set_text(L"downloading...");
	
		const vector<wstring>& markers = _chromium_rel.markers();
	
		dlg_dn_list ddl(_taskBar, _session, _chromium_rel);
		if (ddl.show(hwnd()) == IDOK) {
			_listview.set_redraw(false);
			const int iShown = 1200;
			for (size_t i = markers.size() - iShown; i < markers.size(); ++i) { // display only last markers
				_listview.items.add(markers[i]);
			}
			_lblLoaded.set_text(str::format(L"%d/%d markers (%.2f KB)",
				iShown, markers.size(), static_cast<float>(ddl.get_total_bytes()) / 1024).c_str() );
			_listview.set_redraw(true).column_fit(3);
	
			btnDlList.enable(true);
		}

		SetFocus(_listview.hwnd());
		return TRUE;
	});

	on_command(MNU_MAIN_GETBASIC, [this](params_command p)->INT_PTR
	{
		vector<listview::item> sels = _listview.items.get_selected();
		if (sels.empty()) return TRUE;

		vector<wstring> markers = listview::get_all_text(sels, 0);

		dlg_dn_info ddi(_taskBar, _session, markers);
		if (ddi.show(hwnd()) == IDOK) {
			_listview.set_redraw(false);
			for (size_t i = 0; i < markers.size(); ++i) {
				wstring relDate = str::format(L"%s %s",
					ddi.data[i].releaseDate.substr(0, 10).c_str(),
					ddi.data[i].releaseDate.substr(11, 5).c_str() );
				_listview.items[sels[i].index].set_text(relDate, 1);

				wstring packSz = str::format(L"%.2f MB",
					static_cast<float>(ddi.data[i].packageSize) / 1024 / 1024);
				_listview.items[sels[i].index].set_text(packSz, 2);
			}
			_listview.set_redraw(true);
		}
		return TRUE;
	});

	on_command(MNU_MAIN_GETDLL, [this](params_command p)->INT_PTR
	{
		vector<listview::item> sels = _listview.items.get_selected();
		if (!sels.empty()) {
			if (sels.size() > 1) {
				int q = sys::msg_box(hwnd(), L"Too much download",
					L"You are about to download more than one package.\nThat's a lot of data, proceed?",
					MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2);
				if (q == IDNO) {
					return TRUE; // user aborted download operation
				}
			}

			vector<wstring> markers = listview::get_all_text(sels, 0);
			for (vector<wstring>::size_type i = 0; i < markers.size(); ++i) {
				dlg_dn_dll ddd(_taskBar, _session, markers[i]);
				if (ddd.show(hwnd()) == IDOK) {
					_listview.items[sels[i].index].set_text(ddd.version, 3);
				}
			}
		}
		return TRUE;
	});

	on_command(MNU_MAIN_DLZIP, [this](params_command p)->INT_PTR
	{
		if (_listview.items.count_selected() == 1) {
			dlg_dn_zip ddz(_taskBar, _session, _listview.items.get_selected()[0].get_text());
			ddz.show(hwnd());
		}
		return TRUE;
	});
}