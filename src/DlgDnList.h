
#include "../wolf/wolf.h"
#include "ChromiumRel.h"
using namespace wolf;
using std::vector;
using std::wstring;

// Downloads the list of markers.
class DlgDnList final : public DialogModal {
private:
	Internet::Session& session;
	ChromiumRel&       clist;
	int                totBytes;
	Window             label;
	ProgressBar        progBar;
public:
	DlgDnList(Internet::Session& isess, ChromiumRel& list);
	int getTotalBytes() const { return totBytes; }
private:
	void events() override;
	bool doDownloadList(const wstring& marker);
	bool doReadXml(const vector<BYTE>& buf);
	bool doShowErrAndClose(const wchar_t *msg, const wstring& err);
};