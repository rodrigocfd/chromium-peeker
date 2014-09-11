
#include "../toolow/toolow.h"
#include "ChromiumRel.h"

class DlgDnList final : public DialogModal {
private:
	Internet::Session *pSession;
	ChromiumRel       *pCList;
	int                totBytes;
	Window             label;
	ProgressBar        progBar;
public:
	int show(Window *parent, Internet::Session *session, ChromiumRel *list);
private:
	INT_PTR msgHandler(UINT msg, WPARAM wp, LPARAM lp);
	void onInitDialog();
	void doDownloadList(const wchar_t *marker);
	void doReadXml(const Array<BYTE>& buf);
};