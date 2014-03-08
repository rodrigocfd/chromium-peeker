
#include "../toolow/Dialog.h"
#include "../toolow/Controls.h"

class DlgDownloadDll : public DialogModal {
public:
	int show(Window *parent, const wchar_t *marker);

	String version;

private:
	INT_PTR msgHandler(UINT msg, WPARAM wp, LPARAM lp);
	void on_initDialog();
	void on_webEvent(WPARAM wp, LPARAM lp);
	bool do_readVersion(const Array<BYTE> *pData);
	
	String      marker;
	int         totDownloaded;
	Window      label;
	ProgressBar progBar;
};