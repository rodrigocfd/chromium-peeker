
#include "../toolow/toolow.h"
#include "../res/resource.h"
#include "ChromiumRel.h"

class DlgMain final : public DialogApp {
private:
	ChromiumRel       chromiumRel;
	ListView          listview;
	Window            lblLoaded;
	Resizer           resz;
	Internet::Session session;
public:
	int run(HINSTANCE hInst, int cmdShow) { return DialogApp::run(hInst, cmdShow, DLG_MAIN, ICO_CHROMIUM); }
private:
	INT_PTR msgHandler(UINT msg, WPARAM wp, LPARAM lp) override;
	void onInitDialog();
	void onInitMenuPopup(WPARAM wp);
	void onBtnDownloadList();
	void onMnuBasicDetails();
	void onMnuDllDetails();
	void onMnuDownloadZip();
};