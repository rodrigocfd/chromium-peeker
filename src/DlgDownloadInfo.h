
#include "../toolow/Dialog.h"
#include "../toolow/Controls.h"

class DlgDownloadInfo : public DialogModal {
public:
	struct Data {
		String releaseDate;
		int    packageSize;
	};

	int show(Window *parent, const Array<String> *pMarkers);

	Array<Data> data;

private:
	INT_PTR msgHandler(UINT msg, WPARAM wp, LPARAM lp);
	void on_initDialog();
	void on_webEvent(WPARAM wp, LPARAM lp);
	void do_getOneFile(const wchar_t *marker);
	void do_processFile(const Array<BYTE> *pData);

	const Array<String> *pMarkers;
	int                  totDownloaded;
	Window               label;
	ProgressBar          progBar;
};