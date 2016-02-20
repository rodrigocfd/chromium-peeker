
#include "FrmDn.h"
#include "../winutil/Sys.h"
#include "../res/resource.h"
using std::wstring;

FrmDn::~FrmDn()
{
}

FrmDn::FrmDn(TaskBarProgress& taskBar)
	: _taskBar(taskBar)
{
	setup.dialogId = DLG_PROGRESS;
}

void FrmDn::initControls()
{
	Sys::enableXButton(hwnd(), false);

	_label = { hwnd(), LBL_LBL };

	(_progBar = { hwnd(), PRO_PRO })
		.setRange(0, 100)
		.setPos(0);
}

bool FrmDn::doShowErrAndClose(const wchar_t *msg, const wstring& err)
{
	// Intended to be used form within a separate thread.
	ui_thread([&]()->void {
		_taskBar.setError(true);
		Sys::msgBox(hwnd(), msg, err, MB_ICONERROR);
		_taskBar.clear();
		EndDialog(hwnd(), IDCANCEL);
	});
	return false;
}