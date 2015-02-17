
#include "DlgDn.h"
#include "ChromiumRel.h"
using namespace owl;
using std::vector;
using std::wstring;

// Downloads the list of markers.
class DlgDnList final : public DlgDn {
private:
	Internet::Session& session;
	ChromiumRel&            clist;
	int                     totBytes;
public:
	DlgDnList(Internet::Session& isess, ChromiumRel& list)
		: session(isess), clist(list), totBytes(0) { }
	int getTotalBytes() const { return totBytes; }
private:
	void onInitDialog();
	bool doDownloadList(const wstring& marker);
	bool doReadXml(const vector<BYTE>& buf);

	INT_PTR dlgProc(UINT msg, WPARAM wp, LPARAM lp) override {
		switch (msg) {
		case WM_INITDIALOG: this->onInitDialog(); break;
		}
		return DialogModal::dlgProc(msg, wp, lp);
	}
};