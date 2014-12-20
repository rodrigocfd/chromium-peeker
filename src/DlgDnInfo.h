
#include "../wolf/wolf.h"
using namespace wolf;
using std::vector;
using std::wstring;

// Downloads information for a marker.
class DlgDnInfo final : public DialogModal {
public:
	struct Data final {
		wstring releaseDate;
		int     packageSize;
	};

private:
	Internet::Session&     session;
	const vector<wstring>& markers;
	int                    totDownloaded;
	Window                 label;
	ProgressBar            progBar;
public:
	vector<Data> data;
	DlgDnInfo(Internet::Session& isess, const vector<wstring>& marks);
private:
	void events() override;
	bool doGetOneFile(const wstring& marker);
	bool doProcessFile(const vector<BYTE>& buf);
	bool doShowErrAndClose(const wchar_t *msg, const wstring& err);
};