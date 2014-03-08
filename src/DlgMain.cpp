
#include "DlgMain.h"
#include "../toolow/InitMenuPopup.h"
#include "DlgDownloadList.h"
#include "DlgDownloadInfo.h"
#include "DlgDownloadDll.h"
#include "DlgDownloadZip.h"

INT_PTR DlgMain::msgHandler(UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
	case WM_INITDIALOG:    on_initDialog(); break;
	case WM_SIZE:          this->resizer.doResize(wp, lp); this->listview.columnFit(3); break;
	case WM_INITMENUPOPUP: on_initMenuPopup(wp); break;
	case WM_COMMAND:
		switch(LOWORD(wp))
		{
		case BTN_DLLIST:        do_downloadList(L""); break;
		case IDCANCEL:          this->sendMessage(WM_CLOSE, 0, 0); break;
		case MNU_MAIN_GETBASIC: on_getBasicDetails(); break;
		case MNU_MAIN_GETDLL:   on_getDllDetails(); break;
		case MNU_MAIN_DLZIP:    on_downloadZip(); break;
		}
		break;
	}
	return DialogApp::msgHandler(msg, wp, lp);
}

void DlgMain::on_initDialog()
{
	( this->listview = this->getChild(LST_BUILDS) )
		.setContextMenu(MEN_MAIN)
		.setFullRowSelect()
		.columnAdd(L"Build marker", 80)
		.columnAdd(L"Release date", 105)
		.columnAdd(L"Zip size", 65)
		.columnAdd(L"DLL version", 90)
		.columnFit(3);

	this->resizer.create(2)
		.add({ BTN_DLLIST }, this->hWnd(), Resizer::Do::NOTHING, Resizer::Do::NOTHING)
		.add({ this->listview.hWnd() }, Resizer::Do::RESIZE, Resizer::Do::RESIZE);
	
	this->setWheelHoverBehavior();
}

void DlgMain::on_initMenuPopup(WPARAM wp)
{
	InitMenuPopup menu(wp);

	if(menu.isFirst(MNU_MAIN_GETBASIC)) {
		bool hasSel = this->listview.items.countSelected() >= 1;
		menu.enable(MNU_MAIN_GETBASIC, hasSel)
			.enable(MNU_MAIN_GETDLL, hasSel)
			.enable(MNU_MAIN_DLZIP, this->listview.items.countSelected() == 1);
	}
}

void DlgMain::do_downloadList(const wchar_t *marker)
{
	this->getChild(BTN_DLLIST).setEnable(false);
	this->chromiumReleaseList.reset();
	this->listview.items.removeAll();
	this->listview.columnFit(3);
	
	DlgDownloadList ddll;
	if(ddll.show(this, &this->chromiumReleaseList) == IDOK) {
		const Array<String> *markers = this->chromiumReleaseList.markers();
		this->setText( FMT(L"Chromium Peeker - %d markers", markers->size()) );

		this->listview.setRedraw(false);
		for(int i = markers->size() - 1000; i < markers->size(); ++i) // display only last markers
			this->listview.items.add( (*markers)[i].str() );
		this->listview.setRedraw(true).columnFit(3);
	}

	this->getChild(BTN_DLLIST).setEnable(true);
	this->listview.setFocus();
}

void DlgMain::on_getBasicDetails()
{
	Array<ListView::Item> sels = this->listview.items.getSelected();
	if(!sels.size()) return;
	Array<String> markers = sels.transform<String>(
		[](int i, const ListView::Item& item)->String { return item.getText(0); } );

	DlgDownloadInfo ddli;
	if(ddli.show(this, &markers) == IDOK) {
		this->listview.setRedraw(false);
		for(int i = 0; i < markers.size(); ++i) {
			String relDate = FMT(L"%s %s",
				ddli.data[i].releaseDate.substr(0, 10).str(),
				ddli.data[i].releaseDate.substr(11, 5).str());
			this->listview.items[sels[i].i].setText(relDate.str(), 1);

			String packSz = FMT(L"%.2f MB", (float)ddli.data[i].packageSize / 1024 / 1024);
			this->listview.items[sels[i].i].setText(packSz.str(), 2);
		}
		this->listview.setRedraw(true);
	}
}

void DlgMain::on_getDllDetails()
{
	Array<ListView::Item> sels = this->listview.items.getSelected();
	if(!sels.size()) return;
	Array<String> markers = sels.transform<String>(
		[](int i, const ListView::Item& item)->String { return item.getText(0); } );

	for(int i = 0; i < markers.size(); ++i) {
		DlgDownloadDll ddll;
		if(ddll.show(this, markers[i].str()) == IDOK) {
			this->listview.items[sels[0].i].setText(ddll.version.str(), 3);
		}
	}
}

void DlgMain::on_downloadZip()
{
	if(this->listview.items.countSelected() != 1)
		return;

	DlgDownloadZip ddlz;
	ddlz.show(this, this->listview.items.getSelected()[0].getText().str());
}