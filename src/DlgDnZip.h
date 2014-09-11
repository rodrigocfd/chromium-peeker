
#include "../toolow/toolow.h"

class DlgDnZip final : public DialogModal {
private:
	Internet::Session *pSession;
	String             marker, dest;
	Window             label;
	ProgressBar        progBar;
public:
	int show(Window *parent, Internet::Session *session, const String& marker);
private:
	INT_PTR msgHandler(UINT msg, WPARAM wp, LPARAM lp);
	void onInitDialog();
	void doDownload();
};