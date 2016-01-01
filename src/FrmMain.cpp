
#include "FrmMain.h"
#include "FrmDnDll.h"
#include "FrmDnInfo.h"
#include "FrmDnList.h"
#include "FrmDnZip.h"
#include "../res/resource.h"
using namespace wolf;
using std::vector;
using std::wstring;

RUN(FrmMain);

FrmMain::FrmMain()
	: _resz(this)
{
	setup.dialogId = DLG_MAIN;
	setup.iconId = ICO_CHROMIUM;

	this->onMessage(WM_CREATE, [this](WPARAM wp, LPARAM lp)->LRESULT
	{
		( _listview = this->getChild(LST_BUILDS) )
			.setFullRowSelect()
			.columnAdd(L"Build marker", 80)
			.columnAdd(L"Release date", 105)
			.columnAdd(L"Zip size", 65)
			.columnAdd(L"DLL version", 90)
			.columnFit(3);

		Menu m;
		m.createPopup()
			.addItem(L"Get &details", MNU_MAIN_GETBASIC)
			.addItem(L"Get DLL &version", MNU_MAIN_GETDLL)
			.addItem(L"Download &zip", MNU_MAIN_DLZIP);
		_listview.hMenuContext = m.hMenu();

		_lblLoaded = this->getChild(LBL_LOADED);

		_resz.addById({ BTN_DLLIST }, Resizer::Do::NOTHING, Resizer::Do::NOTHING)
			.addById({ LBL_LOADED }, Resizer::Do::RESIZE, Resizer::Do::NOTHING)
			.addByHwnd({ _listview.hWnd() }, Resizer::Do::RESIZE, Resizer::Do::RESIZE)
			.afterResize([&]() {
				_listview.columnFit(3);
			});

		wstring err;
		if (!_session.init(&err)) { // initialize internet session, for the whole program running time
			Sys::msgBox(this, L"Fail", err, MB_ICONERROR);
			this->sendMessage(WM_CLOSE, 0, 0);
		}
		return 0;
	});

	this->onMessage(WM_INITMENUPOPUP, [this](WPARAM wp, LPARAM lp)->LRESULT
	{
		if (_listview.hMenuContext == reinterpret_cast<HMENU>(wp)) {
			int numSelec = _listview.items.countSelected();
			Menu m = _listview.hMenuContext;
			m.enableItem({ MNU_MAIN_GETBASIC, MNU_MAIN_GETDLL }, numSelec >= 1)
				.enableItem({ MNU_MAIN_DLZIP }, numSelec == 1);
		}
		return 0;
	});

	this->onCommand(BTN_DLLIST, [this]()->LRESULT
	{
		Window btnDlList = this->getChild(BTN_DLLIST);
	
		EnableWindow(btnDlList.hWnd(), TRUE);
		_chromiumRel.reset();
		_listview.items.removeAll();
		_listview.columnFit(3);
		_lblLoaded.setText(L"downloading...");
	
		const vector<wstring>& markers = _chromiumRel.markers();
	
		FrmDnList ddl(this, _session, _chromiumRel);
		ddl.show(this);
	
		_listview.setRedraw(false);
		const int iShown = 1200;
		for (size_t i = markers.size() - iShown; i < markers.size(); ++i) { // display only last markers
			_listview.items.add(markers[i]);
		}
		_lblLoaded.setText(Str::format(L"%d/%d markers (%.2f KB)",
			iShown, markers.size(), static_cast<float>(ddl.getTotalBytes()) / 1024).c_str() );
		_listview.setRedraw(true).columnFit(3);
	
		EnableWindow(btnDlList.hWnd(), TRUE);
		SetFocus(_listview.hWnd());
		return 0;
	});

	this->onCommand(MNU_MAIN_GETBASIC, [this]()->LRESULT
	{
		vector<ListView::Item> sels = _listview.items.getSelected();
		if (sels.empty()) return 0;

		vector<wstring> markers = ListView::getAllText(sels, 0);

		FrmDnInfo ddi(this, _session, markers);
		ddi.show(this);
	
		_listview.setRedraw(false);
		for (size_t i = 0; i < markers.size(); ++i) {
			wstring relDate = Str::format(L"%s %s",
				ddi.data[i].releaseDate.substr(0, 10).c_str(),
				ddi.data[i].releaseDate.substr(11, 5).c_str() );
			_listview.items[sels[i].index].setText(relDate, 1);

			wstring packSz = Str::format(L"%.2f MB",
				static_cast<float>(ddi.data[i].packageSize) / 1024 / 1024);
			_listview.items[sels[i].index].setText(packSz, 2);
		}
		_listview.setRedraw(true);
	
		return 0;
	});

	this->onCommand(MNU_MAIN_GETDLL, [this]()->LRESULT
	{
		vector<ListView::Item> sels = _listview.items.getSelected();
		if (!sels.empty()) {
			if (sels.size() > 1) {
				int q = Sys::msgBox(this, L"Too much download",
					L"You are about to download more than one package.\nThat's a lot of data, proceed?",
					MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2);
				if (q == IDNO) {
					return 0; // user aborted download operation
				}
			}

			vector<wstring> markers = ListView::getAllText(sels, 0);
			for (vector<wstring>::size_type i = 0; i < markers.size(); ++i) {
				FrmDnDll ddd(this, _session, markers[i]);
				ddd.show(this);
				_listview.items[sels[i].index].setText(ddd.version, 3);
			}
		}
		return 0;
	});

	this->onCommand(MNU_MAIN_DLZIP, [this]()->LRESULT
	{
		if (_listview.items.countSelected() == 1) {
			FrmDnZip ddz(this, _session, _listview.items.getSelected()[0].getText());
			ddz.show(this);
		}
		return 0;
	});
}