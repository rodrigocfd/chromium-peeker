
#include "DlgDn.h"

// Downloads information for a marker.
class DlgDnInfo final : public DlgDn {
public:
	struct Data final {
		std::wstring releaseDate;
		int          packageSize;
	};

private:
	c4w::net::Session&               session;
	const std::vector<std::wstring>& markers;
	int                              totDownloaded;
public:
	std::vector<Data> data;
	DlgDnInfo(c4w::net::Session& isess, const std::vector<std::wstring>& marks)
		: session(isess), markers(marks), totDownloaded(0) { }
private:
	void onInitDialog();

	bool doGetOneFile(const std::wstring& marker);
	bool doProcessFile(const std::vector<BYTE>& buf);

	INT_PTR dlgProc(UINT msg, WPARAM wp, LPARAM lp) override {
		switch (msg) {
		case WM_INITDIALOG: onInitDialog(); break;
		}
		return c4w::DialogModal::dlgProc(msg, wp, lp);
	}
};