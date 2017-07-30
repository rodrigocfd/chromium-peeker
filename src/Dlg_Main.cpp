
#include "Dlg_Main.h"
#include "Dlg_Dn_Dll.h"
#include "Dlg_Dn_Info.h"
#include "Dlg_Dn_List.h"
#include "Dlg_Dn_Zip.h"
#include <winlamb-more/menu.h>
#include <winlamb-more/str.h>
#include <winlamb-more/sys.h>
#include <winlamb-more/sysdlg.h>
#include "res/resource.h"
using namespace wl;
using std::vector;
using std::wstring;

RUN(Dlg_Main);

Dlg_Main::Dlg_Main()
	: dialog_main(10), msg_command(10)
{
	setup.dialogId = DLG_MAIN;
	setup.iconId = ICO_CHROMIUM;

	on_message(WM_INITDIALOG, [&](params&)
	{
		m_taskbarProgr.init(this);

		m_lstEntries.assign(this, LST_BUILDS)
			.style.full_row_select(true)
			.set_context_menu(MEN_MAIN)
			.column_add(L"Build marker", 80)
			.column_add(L"Release date", 105)
			.column_add(L"Zip size", 65)
			.column_add(L"DLL version", 90)
			.column_fit(3);

		m_lblLoaded.assign(this, LBL_LOADED);
		m_resz.add(this, LBL_LOADED, resizer::go::RESIZE, resizer::go::NOTHING)
			.add(this, LST_BUILDS, resizer::go::RESIZE, resizer::go::RESIZE);

		wstring err;
		if (!m_session.open(&err)) { // initialize internet session, for the whole program running time
			sysdlg::msgbox(this, L"Fail", err, MB_ICONERROR);
			SendMessage(hwnd(), WM_CLOSE, 0, 0);
		}
		return TRUE;
	});

	on_message(WM_SIZE, [&](params& p)
	{
		m_resz.arrange(p);
		m_lstEntries.column_fit(3);
		return TRUE;
	});

	on_initmenupopup(MNU_MAIN_GETBASIC, [&](wm::initmenupopup p)
	{
		menu m = p.hmenu();
		size_t numSelec = m_lstEntries.items.count_selected();
		m.enable_item({MNU_MAIN_GETBASIC, MNU_MAIN_GETDLL}, numSelec >= 1)
			.enable_item(MNU_MAIN_DLZIP, numSelec == 1);
		return TRUE;
	});

	on_command(BTN_DLLIST, [&](params&)
	{
		EnableWindow(GetDlgItem(hwnd(), BTN_DLLIST), FALSE);
		m_chromiumRel.reset();
		m_lstEntries.items.remove_all();
		m_lstEntries.column_fit(3);
		m_lblLoaded.set_text(L"downloading...");

		const vector<wstring>& markers = m_chromiumRel.markers();

		Dlg_Dn_List ddl(m_taskbarProgr, m_session, m_chromiumRel);
		if (ddl.show(this) == IDOK) {
			m_lstEntries.set_redraw(false);
			const int iShown = 1200;
			for (size_t i = markers.size() - iShown; i < markers.size(); ++i) { // display only last markers
				m_lstEntries.items.add(markers[i]);
			}
			m_lblLoaded.set_text(str::format(L"%d/%d markers (%.2f KB)",
				iShown, markers.size(),
				static_cast<float>(ddl.get_total_bytes()) / 1024).c_str() );
			m_lstEntries.set_redraw(true).column_fit(3);		
			EnableWindow(GetDlgItem(hwnd(), BTN_DLLIST), TRUE);
		}

		m_lstEntries.focus();
		return TRUE;
	});

	on_command(MNU_MAIN_GETBASIC, [&](params&)
	{
		vector<listview::item> sels = m_lstEntries.items.get_selected();
		if (sels.empty()) return TRUE;

		vector<wstring> markers = m_lstEntries.items.get_texts(sels, 0);

		Dlg_Dn_Info ddi(m_taskbarProgr, m_session, markers);
		if (ddi.show(this) == IDOK) {
			m_lstEntries.set_redraw(false);
			for (size_t i = 0; i < markers.size(); ++i) {
				wstring relDate = str::format(L"%s %s",
					ddi.data[i].releaseDate.substr(0, 10).c_str(),
					ddi.data[i].releaseDate.substr(11, 5).c_str() );
				m_lstEntries.items[sels[i].index].set_text(relDate, 1);

				wstring packSz = str::format(L"%.2f MB",
					static_cast<float>(ddi.data[i].packageSize) / 1024 / 1024);
				m_lstEntries.items[sels[i].index].set_text(packSz, 2);
			}
			m_lstEntries.set_redraw(true);
		}
		return TRUE;
	});

	on_command(MNU_MAIN_GETDLL, [&](params&)
	{
		vector<listview::item> sels = m_lstEntries.items.get_selected();
		if (!sels.empty()) {
			if (sels.size() > 1) {
				int q = sysdlg::msgbox(this, L"Too much download",
					L"You are about to download more than one package.\n"
					L"That's a lot of data, proceed?",
					MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2);
				if (q == IDNO) {
					return TRUE; // user aborted download operation
				}
			}

			vector<wstring> markers = m_lstEntries.items.get_texts(sels, 0);
			for (vector<wstring>::size_type i = 0; i < markers.size(); ++i) {
				Dlg_Dn_Dll ddd(m_taskbarProgr, m_session, markers[i]);
				if (ddd.show(this) == IDOK) {
					m_lstEntries.items[sels[i].index].set_text(ddd.version, 3);
				}
			}
		}
		return TRUE;
	});

	on_command(MNU_MAIN_DLZIP, [&](params&)
	{
		if (m_lstEntries.items.count_selected() == 1) {
			Dlg_Dn_Zip ddz(m_taskbarProgr, m_session,
				m_lstEntries.items.get_selected()[0].get_text());
			ddz.show(this);
		}
		return TRUE;
	});
}