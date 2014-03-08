
#include "../toolow/Dialog.h"
#include "../toolow/Controls.h"

class DlgDownloadZip : public DialogModal {
public:
	int show(Window *parent, const wchar_t *marker);

private:
	INT_PTR msgHandler(UINT msg, WPARAM wp, LPARAM lp);
	void on_initDialog();
	void on_webEvent(WPARAM wp, LPARAM lp);
	
	String      marker, dest;
	Window      label;
	ProgressBar progBar;
};