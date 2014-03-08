
#include "../toolow/Dialog.h"
#include "../toolow/Controls.h"
#include "../toolow/ListView.h"
#include "../toolow/Resizer.h"
#include "../res/resource.h"
#include "ChromiumReleaseList.h"

class DlgMain : public DialogApp {
public:
	int run(HINSTANCE hInst, int cmdShow) { return DialogApp::run(hInst, cmdShow, DLG_MAIN, ICO_CHROMIUM); }

private:
	ChromiumReleaseList chromiumReleaseList;
	ProgressBar         progbar;
	ListView            listview;
	Resizer             resizer;

	INT_PTR msgHandler(UINT msg, WPARAM wp, LPARAM lp);
	void on_initDialog();
	void on_initMenuPopup(WPARAM wp);
	void on_getBasicDetails();
	void on_getDllDetails();
	void on_downloadZip();

	void do_downloadList(const wchar_t *marker);
};