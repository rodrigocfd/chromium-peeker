
#include "../wolf/wolf.h"
using namespace wolf;
using std::wstring;

// Downloads DLL version for a marker.
class DlgDnDll final : public DialogModal {
private:
	Internet::Session& session;
	wstring            marker;
	int                totDownloaded;
	Window             label;
	ProgressBar        progBar;
public:
	wstring version;
	DlgDnDll(Internet::Session& isess, const wstring& mark);
private:
	void events() override;
	bool doDownload();
	bool doReadVersion(wstring zipPath);
	bool doShowErrAndClose(const wchar_t *msg, const wstring& err);
};