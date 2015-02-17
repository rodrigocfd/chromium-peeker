
#include "../owl/owl.h"
#include "../res/resource.h"
#include "ChromiumRel.h"
using namespace owl;

class DlgMain final : public DialogApp {
private:
	ChromiumRel       chromiumRel;
	ListView          listview;
	Window            lblLoaded;
	Resizer           resz;
	Internet::Session session;

public:
	DlgMain() : DialogApp(DLG_MAIN, ICO_CHROMIUM) { }
private:
	void onInitDialog();
	void onInitMenuPopup(WPARAM wp);
	void onBtnDlList();
	void onMnuMainGetBasic();
	void onMnuMainGetDll();
	void onMnuMainDlZip();

	INT_PTR dlgProc(UINT msg, WPARAM wp, LPARAM lp) override {
		switch (msg) {
		case WM_INITDIALOG:    this->onInitDialog();      break;
		case WM_INITMENUPOPUP: this->onInitMenuPopup(wp); return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wp)) {
			case BTN_DLLIST:        this->onBtnDlList();       return TRUE;
			case MNU_MAIN_GETBASIC: this->onMnuMainGetBasic(); return TRUE;
			case MNU_MAIN_GETDLL:   this->onMnuMainGetDll();   return TRUE;
			case MNU_MAIN_DLZIP:    this->onMnuMainDlZip();    return TRUE;
			}
		}
		return DialogApp::dlgProc(msg, wp, lp);
	}
};