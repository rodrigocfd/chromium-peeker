
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

	window_dialog_modal::on_message(WM_INITDIALOG, [this](WPARAM wp, LPARAM lp)->INT_PTR
	{
		Sys::enableXButton(hwnd(), false);

		_label = GetDlgItem(hwnd(), LBL_LBL);
		(_progBar = GetDlgItem(hwnd(), PRO_PRO))
			.setRange(0, 100)
			.setPos(0);
		
		return _userInitDialog ? _userInitDialog(wp, lp) : TRUE;
	});
}

void FrmDn::on_message(UINT msg, msg_func_type callback)
{
	if (msg == WM_INITDIALOG) _userInitDialog = std::move(callback);
	else window_dialog_modal::on_message(msg, std::move(callback));
}

bool FrmDn::doShowErrAndClose(const wchar_t *msg, const wstring& err)
{
	// Intended to be used form within a separate thread.
	gui_thread([&]()->void {
		_taskBar.setError(true);
		Sys::msgBox(hwnd(), msg, err, MB_ICONERROR);
		_taskBar.dismiss();
		EndDialog(hwnd(), IDCANCEL);
	});
	return false;
}