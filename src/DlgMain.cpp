
#include "DlgMain.h"
#include "DlgDnDll.h"
#include "DlgDnInfo.h"
#include "DlgDnList.h"
#include "DlgDnZip.h"

RUN(DlgMain);

INT_PTR DlgMain::msgHandler(UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
	case WM_INITDIALOG:    this->onInitDialog(); break;
	case WM_INITMENUPOPUP: this->onInitMenuPopup(wp); break;
	case WM_COMMAND:
		switch(LOWORD(wp))
		{
		case IDCANCEL:          this->sendMessage(WM_CLOSE, 0, 0); break;
		case BTN_DLLIST:        this->onBtnDownloadList(); break;
		case MNU_MAIN_GETBASIC: this->onMnuBasicDetails(); break;
		case MNU_MAIN_GETDLL:   this->onMnuDllDetails(); break;
		case MNU_MAIN_DLZIP:    this->onMnuDownloadZip(); break;
		}
		break;
	}
	return DialogApp::msgHandler(msg, wp, lp);
}

void DlgMain::onInitDialog()
{
	( this->listview = this->getChild(LST_BUILDS) )
		.setContextMenu(MEN_MAIN)
		.setFullRowSelect()
		.columnAdd(L"Build marker", 80)
		.columnAdd(L"Release date", 105)
		.columnAdd(L"Zip size", 65)
		.columnAdd(L"DLL version", 90)
		.columnFit(3);

	this->lblLoaded = this->getChild(LBL_LOADED);

	this->resz.create(2)
		.add({ BTN_DLLIST, LBL_LOADED }, this->hWnd(), Resizer::Do::NOTHING, Resizer::Do::NOTHING)
		.add({ this->listview.hWnd() }, Resizer::Do::RESIZE, Resizer::Do::RESIZE)
		.afterResize([&]() { this->listview.columnFit(3); });

	String err;
	if(!this->session.init(&err)) { // initialize internet session, for the whole program running time
		this->messageBox(L"Fail", err, MB_ICONERROR);
		this->endDialog(IDCANCEL);
	}
}

void DlgMain::onInitMenuPopup(WPARAM wp)
{
	InitMenuPopup menu(wp);

	if(menu.isFirst(MNU_MAIN_GETBASIC)) {
		bool hasSel = this->listview.items.countSelected() >= 1;
		menu.enable(MNU_MAIN_GETBASIC, hasSel)
			.enable(MNU_MAIN_GETDLL, hasSel)
			.enable(MNU_MAIN_DLZIP, this->listview.items.countSelected() == 1);
	}
}

void DlgMain::onBtnDownloadList()
{
	this->getChild(BTN_DLLIST).setEnable(false);
	this->chromiumRel.reset();
	this->listview.items.removeAll();
	this->listview.columnFit(3);
	
	DlgDnList ddl;
	if(ddl.show(this, &this->session, &this->chromiumRel) == IDOK) {
		const Array<String>& markers = this->chromiumRel.markers();

		this->listview.setRedraw(false);
		const int iShown = 1100;
		for(int i = markers.size() - iShown; i < markers.size(); ++i) // display only last markers
			this->listview.items.add(markers[i]);

		this->lblLoaded.setText( String::Fmt(L"%d/%d markers", iShown, markers.size()) );
		this->listview.setRedraw(true).columnFit(3);
	}
	
	this->getChild(BTN_DLLIST).setEnable(true);
	this->listview.setFocus();
}

void DlgMain::onMnuBasicDetails()
{
	Array<ListView::Item> sels = this->listview.items.getSelected();
	if(!sels.size()) return;
	Array<String> markers = sels.transform<String>(
		[](int i, const ListView::Item& item)->String { return item.getText(0); } );

	DlgDnInfo ddi;
	if(ddi.show(this, &this->session, &markers) == IDOK) {
		this->listview.setRedraw(false);
		markers.each([&](int i, const String& marker) {
			String relDate = String::Fmt(L"%s %s",
				ddi.data[i].releaseDate.substr(0, 10).str(),
				ddi.data[i].releaseDate.substr(11, 5).str() );
			this->listview.items[sels[i].i].setText(relDate, 1);

			String packSz = String::Fmt(L"%.2f MB", (float)ddi.data[i].packageSize / 1024 / 1024);
			this->listview.items[sels[i].i].setText(packSz, 2);
		});
		this->listview.setRedraw(true);
	}
}

void DlgMain::onMnuDllDetails()
{
	Array<ListView::Item> sels = this->listview.items.getSelected();
	if(!sels.size()) return;
	if(sels.size() > 1) {
		if(this->messageBox(L"Too much download",
			L"You are about to download more than one package.\nThat's a lot of data, proceed?",
			MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2) == IDNO) return;
	}

	Array<String> markers = sels.transform<String>(
		[](int i, const ListView::Item& item)->String { return item.getText(0); } );

	markers.each([&](int itemNo, const String& marker) {
		DlgDnDll ddd;
		if(ddd.show(this, &this->session, marker) == IDOK)
			this->listview.items[sels[itemNo].i].setText(ddd.version, 3);
	});
}

void DlgMain::onMnuDownloadZip()
{
	if(this->listview.items.countSelected() != 1)
		return;

	DlgDnZip ddz;
	ddz.show(this, &this->session, this->listview.items.getSelected()[0].getText());
}