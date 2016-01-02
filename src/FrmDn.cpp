
#include "FrmDn.h"
#include "../res/resource.h"
using namespace wolf;
using std::wstring;

FrmDn::~FrmDn()
{
}

FrmDn::FrmDn(TaskBarProgress& taskBar)
	: _taskBar(taskBar)
{
	setup.dialogId = DLG_PROGRESS;

	this->onMessage(WM_CREATE, [this](WPARAM wp, LPARAM lp)->LRESULT
	{
		EnableMenuItem(GetSystemMenu(this->hWnd(), FALSE),
			SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED); // disable X button

		_label = this->getChild(LBL_LBL);
		(_progBar = this->getChild(PRO_PRO))
			.setRange(0, 100)
			.setPos(0);
		return 0;
	});
}

bool FrmDn::_doShowErrAndClose(const wchar_t *msg, const wstring& err)
{
	// Intended to be used form within a separate thread.
	this->guiThread([&]()->void {
		_taskBar.setError(true);
		Sys::msgBox(this, msg, err, MB_ICONERROR);
		_taskBar.dismiss();
		this->sendMessage(WM_CLOSE, 0, 0);
	});
	return false;
}