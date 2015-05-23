
#include "../c4w/c4w.h"
#include "../res/resource.h"
#include "ChromiumRel.h"
using namespace c4w;

class DlgMain final : public DialogApp {
private:
	ChromiumRel  chromiumRel;
	ListView     listview;
	Window       lblLoaded;
	Resizer      resz;
	net::Session session;
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
		case WM_INITDIALOG:    onInitDialog();      break;
		case WM_INITMENUPOPUP: onInitMenuPopup(wp); return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wp)) {
			case BTN_DLLIST:        onBtnDlList();       return TRUE;
			case MNU_MAIN_GETBASIC: onMnuMainGetBasic(); return TRUE;
			case MNU_MAIN_GETDLL:   onMnuMainGetDll();   return TRUE;
			case MNU_MAIN_DLZIP:    onMnuMainDlZip();    return TRUE;
			}
		}
		return DialogApp::dlgProc(msg, wp, lp);
	}
};