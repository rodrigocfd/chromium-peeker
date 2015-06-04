
#include "DlgMain.h"
#include "DlgDnDll.h"
#include "DlgDnInfo.h"
#include "DlgDnList.h"
#include "DlgDnZip.h"
using namespace wolf;
using namespace wolf::ctrl;
using std::vector;
using std::wstring;

RUN(DlgMain);
void DlgMain::events() {

this->onMessage(WM_INITDIALOG, [&](WPARAM wp, LPARAM lp)->INT_PTR
{
	( _listview = this->getChild(LST_BUILDS) )
		.setFullRowSelect()
		.columnAdd(L"Build marker", 80)
		.columnAdd(L"Release date", 105)
		.columnAdd(L"Zip size", 65)
		.columnAdd(L"DLL version", 90)
		.columnFit(3)
		.contextMenu
			.addItem(L"Get &details", MNU_MAIN_GETBASIC)
			.addItem(L"Get DLL &version", MNU_MAIN_GETDLL)
			.addItem(L"Download &zip", MNU_MAIN_DLZIP);

	_lblLoaded = this->getChild(LBL_LOADED);

	_resz.add({ BTN_DLLIST }, this, Resizer::Do::NOTHING, Resizer::Do::NOTHING)
		.add({ LBL_LOADED }, this, Resizer::Do::RESIZE, Resizer::Do::NOTHING)
		.add({ _listview }, Resizer::Do::RESIZE, Resizer::Do::RESIZE)
		.afterResize([&]() {
			_listview.columnFit(3);
		});

	wstring err;
	if (!_session.init(&err)) { // initialize internet session, for the whole program running time
		this->messageBox(L"Fail", err, MB_ICONERROR);
		this->sendMessage(WM_CLOSE, 0, 0);
	}
	return TRUE;
});

this->onMessage(WM_INITMENUPOPUP, [&](WPARAM wp, LPARAM lp)->INT_PTR
{
	if (_listview.contextMenu.hMenu() == reinterpret_cast<HMENU>(wp)) {
		int numSelec = _listview.items.countSelected();
		_listview.contextMenu
			.enableItem({ MNU_MAIN_GETBASIC, MNU_MAIN_GETDLL }, numSelec >= 1)
			.enableItem({ MNU_MAIN_DLZIP }, numSelec == 1);
	}
	return TRUE;
});

this->onCommand(BTN_DLLIST, [&]()->INT_PTR
{
	this->getChild(BTN_DLLIST).setEnable(false);
	_chromiumRel.reset();
	_listview.items.removeAll();
	_listview.columnFit(3);
	_lblLoaded.setText(L"downloading...");
	
	DlgDnList ddl(_session, _chromiumRel);
	if (ddl.show(this) == IDOK) {
		const vector<wstring>& markers = _chromiumRel.markers();

		_listview.setRedraw(false);
		const int iShown = 1200;
		for (size_t i = markers.size() - iShown; i < markers.size(); ++i) { // display only last markers
			_listview.items.add(markers[i]);
		}
		_lblLoaded.setText( str::Sprintf(L"%d/%d markers (%.2f KB)",
			iShown, markers.size(), static_cast<float>(ddl.getTotalBytes()) / 1024) );
		_listview.setRedraw(true).columnFit(3);
	}
	
	this->getChild(BTN_DLLIST).setEnable(true);
	_listview.setFocus();
	return TRUE;
});

this->onCommand(MNU_MAIN_GETBASIC, [&]()->INT_PTR
{
	vector<ListView::Item> sels = _listview.items.getSelected();
	if (sels.empty()) return TRUE;

	vector<wstring> markers = ListView::TextsFromItems(sels, 0);

	DlgDnInfo ddi(_session, markers);
	if (ddi.show(this) == IDOK) {
		_listview.setRedraw(false);

		for (size_t i = 0; i < markers.size(); ++i) {
			wstring relDate = str::Sprintf(L"%s %s",
				ddi.data[i].releaseDate.substr(0, 10).c_str(),
				ddi.data[i].releaseDate.substr(11, 5).c_str() );
			_listview.items[sels[i].i].setText(relDate, 1);

			wstring packSz = str::Sprintf(L"%.2f MB",
				static_cast<float>(ddi.data[i].packageSize) / 1024 / 1024);
			_listview.items[sels[i].i].setText(packSz, 2);
		}
		_listview.setRedraw(true);
	}
	return TRUE;
});

this->onCommand(MNU_MAIN_GETDLL, [&]()->INT_PTR
{
	vector<ListView::Item> sels = _listview.items.getSelected();
	if (sels.empty()) return TRUE;
	if (sels.size() > 1) {
		if (this->messageBox(L"Too much download",
			L"You are about to download more than one package.\nThat's a lot of data, proceed?",
			MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2) == IDNO) return TRUE;
	}

	vector<wstring> markers = ListView::TextsFromItems(sels, 0);

	for (vector<wstring>::size_type i = 0; i < markers.size(); ++i) {
		DlgDnDll ddd(_session, markers[i]);
		if (ddd.show(this) == IDOK) {
			_listview.items[sels[i].i].setText(ddd.version, 3);
		}
	}
	return TRUE;
});

this->onCommand(MNU_MAIN_DLZIP, [&]()->INT_PTR
{
	if (_listview.items.countSelected() != 1) return TRUE;

	DlgDnZip ddz(_session, _listview.items.getSelected()[0].getText());
	ddz.show(this);
	return TRUE;
});

}//events