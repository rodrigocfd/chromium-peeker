
#include "DlgDn.h"
using namespace c4w;
using std::wstring;

// Download the marker zip file to disk, destination chosen by user.
class DlgDnZip final : public DlgDn {
private:
	net::Session& session;
	wstring       marker, dest;
public:
	DlgDnZip(net::Session& isess, const wstring& mark)
		: session(isess), marker(mark) { }
private:
	void onInitDialog();

	bool doDownload();
	bool doReceiveData(net::Download& zipdl, file::Raw& fout);

	INT_PTR dlgProc(UINT msg, WPARAM wp, LPARAM lp) override {
		switch (msg) {
		case WM_INITDIALOG: onInitDialog(); break;
		}
		return DialogModal::dlgProc(msg, wp, lp);
	}
};