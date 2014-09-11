
#include "../toolow/toolow.h"

class DlgDnDll final : public DialogModal {
private:
	Internet::Session *pSession;
	String             marker;
	int                totDownloaded;
	Window             label;
	ProgressBar        progBar;
public:
	String version;
	int show(Window *parent, Internet::Session *session, const String& marker);
private:
	INT_PTR msgHandler(UINT msg, WPARAM wp, LPARAM lp);
	void onInitDialog();
	void doDownload();
	void doReadVersion(String zipPath);
};