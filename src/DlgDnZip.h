
#include "DlgDn.h"
using namespace owl;
using std::wstring;

// Download the marker zip file to disk, destination chosen by user.
class DlgDnZip final : public DlgDn {
private:
	Internet::Session& session;
	wstring            marker, dest;
public:
	DlgDnZip(Internet::Session& isess, const wstring& mark)
		: session(isess), marker(mark) { }
private:
	void onInitDialog();
	bool doDownload();
	bool doReceiveData(Internet::Download& zipdl, File::Raw& fout);

	INT_PTR dlgProc(UINT msg, WPARAM wp, LPARAM lp) override {
		switch (msg) {
		case WM_INITDIALOG: this->onInitDialog(); break;
		}
		return DialogModal::dlgProc(msg, wp, lp);
	}
};