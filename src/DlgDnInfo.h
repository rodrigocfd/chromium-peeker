
#include "../toolow/toolow.h"

class DlgDnInfo final : public DialogModal {
public:
	struct Data final {
		String releaseDate;
		int    packageSize;
	};

private:
	Internet::Session   *pSession;
	const Array<String> *pMarkers;
	int                  totDownloaded;
	Window               label;
	ProgressBar          progBar;
public:
	Array<Data> data;
	int show(Window *parent, Internet::Session *session, const Array<String> *pMarkers);
private:
	INT_PTR msgHandler(UINT msg, WPARAM wp, LPARAM lp);
	void onInitDialog();
	void doGetOneFile(const wchar_t *marker);
	void doProcessFile(const Array<BYTE>& buf);
};