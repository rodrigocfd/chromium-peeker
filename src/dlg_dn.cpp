
#include "dlg_dn.h"
#include "../winutil/sys.h"
#include "../res/resource.h"
using namespace winutil;
using std::wstring;

dlg_dn::~dlg_dn()
{
}

dlg_dn::dlg_dn(taskbar_progress& taskBar)
	: _taskBar(taskBar)
{
	setup.dialogId = DLG_PROGRESS;
}

void dlg_dn::init_controls()
{
	center_on_parent();
	sys::enable_x_button(hwnd(), false);

	_label = GetDlgItem(hwnd(), LBL_LBL);

	(_progBar = GetDlgItem(hwnd(), PRO_PRO))
		.set_range(0, 100)
		.set_pos(0);
}

bool dlg_dn::show_err_and_close(const wchar_t* msg, const wstring& err)
{
	// Intended to be used form within a separate thread.
	on_ui_thread([&]()->void {
		_taskBar.set_error(true);
		sys::msg_box(hwnd(), msg, err, MB_ICONERROR);
		_taskBar.clear();
		EndDialog(hwnd(), IDCANCEL);
	});
	return false;
}