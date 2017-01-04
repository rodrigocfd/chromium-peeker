
#include "Dlg_Main.h"
#include "Dlg_Dn_Dll.h"
#include "Dlg_Dn_Info.h"
#include "Dlg_Dn_List.h"
#include "Dlg_Dn_Zip.h"
#include "../winlamb/menu.h"
#include "../winlamb/str.h"
#include "../winlamb/sys.h"
#include "../winlamb/sysdlg.h"
#include "../res/resource.h"
using namespace wl;
using std::vector;
using std::wstring;

RUN(Dlg_Main);

Dlg_Main::Dlg_Main()
{
	setup.dialogId = DLG_MAIN;
	setup.iconId = ICO_CHROMIUM;

	on.INITDIALOG([&](params::initdialog p)
	{
		_taskBar.init(hwnd());
		_listview.be(hwnd(), LST_BUILDS)
			.set_full_row_select()
			.set_context_menu(MEN_MAIN)
			.column_add(L"Build marker", 80)
			.column_add(L"Release date", 105)
			.column_add(L"Zip size", 65)
			.column_add(L"DLL version", 90)
			.column_fit(3);
		_lblLoaded.be(hwnd(), LBL_LOADED);
		_resizer.add(hwnd(), LBL_LOADED, resizer::go::RESIZE, resizer::go::NOTHING)
			.add(hwnd(), LST_BUILDS, resizer::go::RESIZE, resizer::go::RESIZE);

		wstring err;
		if (!_session.open(&err)) { // initialize internet session, for the whole program running time
			sysdlg::msgbox(hwnd(), L"Fail", err, MB_ICONERROR);
			SendMessage(hwnd(), WM_CLOSE, 0, 0);
		}
		return TRUE;
	});

	on.SIZE([&](params::size p)
	{
		_resizer.arrange(p);
		_listview.column_fit(3);
		return TRUE;
	});

	on.INITMENUPOPUP([&](params::initmenupopup p)
	{
		menu m(p.hmenu());
		if (m.get_command_id(0) == MNU_MAIN_GETBASIC) {
			size_t numSelec = _listview.items.count_selected();
			m.enable_item({ MNU_MAIN_GETBASIC, MNU_MAIN_GETDLL }, numSelec >= 1)
				.enable_item(MNU_MAIN_DLZIP, numSelec == 1);
			return TRUE;
		}
		return FALSE;
	});

	on.COMMAND(BTN_DLLIST, [&](params::command p)
	{
		EnableWindow(GetDlgItem(hwnd(), BTN_DLLIST), FALSE);
		_chromiumRel.reset();
		_listview.items.remove_all();
		_listview.column_fit(3);
		_lblLoaded.set_text(L"downloading...");

		const vector<wstring>& markers = _chromiumRel.markers();

		Dlg_Dn_List ddl(_taskBar, _session, _chromiumRel);
		if (ddl.show(hwnd()) == IDOK) {
			_listview.set_redraw(false);
			const int iShown = 1200;
			for (size_t i = markers.size() - iShown; i < markers.size(); ++i) { // display only last markers
				_listview.items.add(markers[i]);
			}
			_lblLoaded.set_text(str::format(L"%d/%d markers (%.2f KB)",
				iShown, markers.size(),
				static_cast<float>(ddl.get_total_bytes()) / 1024).c_str() );
			_listview.set_redraw(true).column_fit(3);		
			EnableWindow(GetDlgItem(hwnd(), BTN_DLLIST), TRUE);
		}

		_listview.focus();
		return TRUE;
	});

	on.COMMAND(MNU_MAIN_GETBASIC, [&](params::command p)
	{
		vector<listview::item> sels = _listview.items.get_selected();
		if (sels.empty()) return TRUE;

		vector<wstring> markers = listview::get_all_text(sels, 0);

		Dlg_Dn_Info ddi(_taskBar, _session, markers);
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

	on.COMMAND(MNU_MAIN_GETDLL, [&](params::command p)
	{
		vector<listview::item> sels = _listview.items.get_selected();
		if (!sels.empty()) {
			if (sels.size() > 1) {
				int q = sysdlg::msgbox(hwnd(), L"Too much download",
					L"You are about to download more than one package.\n"
					L"That's a lot of data, proceed?",
					MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2);
				if (q == IDNO) {
					return TRUE; // user aborted download operation
				}
			}

			vector<wstring> markers = listview::get_all_text(sels, 0);
			for (vector<wstring>::size_type i = 0; i < markers.size(); ++i) {
				Dlg_Dn_Dll ddd(_taskBar, _session, markers[i]);
				if (ddd.show(hwnd()) == IDOK) {
					_listview.items[sels[i].index].set_text(ddd.version, 3);
				}
			}
		}
		return TRUE;
	});

	on.COMMAND(MNU_MAIN_DLZIP, [&](params::command p)
	{
		if (_listview.items.count_selected() == 1) {
			Dlg_Dn_Zip ddz(_taskBar, _session,
				_listview.items.get_selected()[0].get_text());
			ddz.show(hwnd());
		}
		return TRUE;
	});
}