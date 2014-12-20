
#include "../wolf/wolf.h"
using namespace wolf;
using std::wstring;

// Download the marker zip file to disk, destination chosen by user.
class DlgDnZip final : public DialogModal {
private:
	Internet::Session& session;
	wstring            marker, dest;
	Window             label;
	ProgressBar        progBar;
public:
	DlgDnZip(Internet::Session& isess, const wstring& mark);
private:
	void events() override;
	bool doDownload();
	bool doReceiveData(Internet::Download& zipdl, File::Raw& fout);
	bool doShowErrAndClose(const wchar_t *msg, const wstring& err);
};