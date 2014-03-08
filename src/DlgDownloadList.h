
#include "../toolow/Dialog.h"
#include "../toolow/Controls.h"
#include "ChromiumReleaseList.h"

class DlgDownloadList : public DialogModal {
public:
	int show(Window *parent, ChromiumReleaseList *pList);

private:
	INT_PTR msgHandler(UINT msg, WPARAM wp, LPARAM lp);
	void on_initDialog();
	void on_webEvent(WPARAM wp, LPARAM lp);
	void do_downloadList(const wchar_t *marker);
	void do_readXml(const Array<BYTE> *buf);

	ChromiumReleaseList *pCList;
	int                  totBytes;
	Window               label;
	ProgressBar          progBar;
};