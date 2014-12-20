
#include <algorithm>
#include "DlgMain.h"
#include "../res/resource.h"
#include "DlgDnDll.h"
#include "DlgDnInfo.h"
#include "DlgDnList.h"
#include "DlgDnZip.h"
using std::vector;
using std::wstring;

RUN(DlgMain);

void DlgMain::events()
{
	this->defineDialog(DLG_MAIN);
	this->defineIcon(ICO_CHROMIUM);
	this->listMenu.defineOwner(this);

	this->onInitDialog([&]() {
		( this->listview = this->getChild(LST_BUILDS) )
			.setFullRowSelect()
			.columnAdd(L"Build marker", 80)
			.columnAdd(L"Release date", 105)
			.columnAdd(L"Zip size", 65)
			.columnAdd(L"DLL version", 90)
			.columnFit(3)
			.setContextMenu(this->listMenu);

		this->lblLoaded = this->getChild(LBL_LOADED);

		this->resz.add({ BTN_DLLIST }, this->hWnd(), Resizer::Do::NOTHING, Resizer::Do::NOTHING)
			.add({ LBL_LOADED }, this->hWnd(), Resizer::Do::RESIZE, Resizer::Do::NOTHING)
			.add({ this->listview.hWnd() }, Resizer::Do::RESIZE, Resizer::Do::RESIZE)
			.afterResize([&]() { this->listview.columnFit(3); });

		wstring err;
		if (!this->session.init(&err)) { // initialize internet session, for the whole program running time
			this->messageBox(L"Fail", err, MB_ICONERROR);
			this->sendMessage(WM_CLOSE, 0, 0);
		}
	});

	this->onInitMenuPopup([&](HMENU hMenu) {
		if (this->listMenu.hMenu() == hMenu) {
			int numSelec = this->listview.items.countSelected();
			this->listMenu.enableItem({ 0, 1 }, numSelec >= 1)
				.enableItem({ 2 }, numSelec == 1);
		}
	});

	this->onCommand(BTN_DLLIST, [&]() {
		this->getChild(BTN_DLLIST).setEnable(false);
		this->chromiumRel.reset();
		this->listview.items.removeAll();
		this->listview.columnFit(3);
	
		DlgDnList ddl(this->session, this->chromiumRel);
		if (ddl.show(this) == IDOK) {
			const vector<wstring>& markers = this->chromiumRel.markers();

			this->listview.setRedraw(false);
			const int iShown = 1200;
			for (vector<wstring>::size_type i = markers.size() - iShown; i < markers.size(); ++i) // display only last markers
				this->listview.items.add(markers[i]);

			this->lblLoaded.setText( Sprintf(L"%d/%d markers (%.2f KB)",
				iShown, markers.size(), (float)ddl.getTotalBytes() / 1024) );
			this->listview.setRedraw(true).columnFit(3);
		}
	
		this->getChild(BTN_DLLIST).setEnable(true);
		this->listview.setFocus();
	});

	this->listMenu.appendItem(L"Get &details", [&]() {
		vector<ListView::Item> sels = this->listview.items.getSelected();
		if (!sels.size()) return;

		vector<wstring> markers(sels.size());
		std::transform(sels.begin(), sels.end(), markers.begin(),
			[](const ListView::Item& item) { return item.getText(0); });

		DlgDnInfo ddi(this->session, markers);
		if (ddi.show(this) == IDOK) {
			this->listview.setRedraw(false);

			for (vector<wstring>::size_type i = 0; i < markers.size(); ++i) {
				wstring relDate = Sprintf(L"%s %s",
					ddi.data[i].releaseDate.substr(0, 10).c_str(),
					ddi.data[i].releaseDate.substr(11, 5).c_str() );
				this->listview.items[sels[i].i].setText(relDate, 1);

				wstring packSz = Sprintf(L"%.2f MB", (float)ddi.data[i].packageSize / 1024 / 1024);
				this->listview.items[sels[i].i].setText(packSz, 2);
			}
			this->listview.setRedraw(true);
		}
	});

	this->listMenu.appendItem(L"Get DLL &version", [&]() {
		vector<ListView::Item> sels = this->listview.items.getSelected();
		if (!sels.size()) return;
		if (sels.size() > 1) {
			if (this->messageBox(L"Too much download",
				L"You are about to download more than one package.\nThat's a lot of data, proceed?",
				MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2) == IDNO) return;
		}

		vector<wstring> markers(sels.size());
		std::transform(sels.begin(), sels.end(), markers.begin(),
			[](const ListView::Item& item) { return item.getText(0); });

		for (vector<wstring>::size_type i = 0; i < markers.size(); ++i) {
			DlgDnDll ddd(this->session, markers[i]);
			if (ddd.show(this) == IDOK)
				this->listview.items[sels[i].i].setText(ddd.version, 3);
		}
	});

	this->listMenu.appendItem(L"Download &zip", [&]() {
		if (this->listview.items.countSelected() != 1)
			return;

		DlgDnZip ddz(this->session, this->listview.items.getSelected()[0].getText());
		ddz.show(this);
	});
}