
#include "DlgDn.h"
using namespace owl;
using std::wstring;

// Downloads DLL version for a marker.
class DlgDnDll final : public DlgDn {
private:
	Internet::Session& session;
	wstring            marker;
	int                totDownloaded;
public:
	wstring version;
	DlgDnDll(Internet::Session& isess, const wstring& mark)
		: session(isess), marker(mark), totDownloaded(0) { }
private:
	void onInitDialog();
	bool doDownload();
	bool doReadVersion(wstring zipPath);

	INT_PTR dlgProc(UINT msg, WPARAM wp, LPARAM lp) override {
		switch (msg) {
		case WM_INITDIALOG: this->onInitDialog(); break;
		}
		return DialogModal::dlgProc(msg, wp, lp);
	}
};