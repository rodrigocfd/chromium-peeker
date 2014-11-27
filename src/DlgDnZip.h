
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
	INT_PTR msgHandler(UINT msg, WPARAM wp, LPARAM lp) override;
	void onInitDialog();
	bool doDownload();
	bool doReceiveData(Internet::Download& zipdl, File::Raw& fout);
	bool doShowErrAndClose(const wchar_t *msg, const String& err);
};