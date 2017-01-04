
#include "Dlg_Dn.h"
#include "../winlamb/sysdlg.h"
#include "../res/resource.h"
using namespace wl;
using std::wstring;

Dlg_Dn::~Dlg_Dn()
{
}

Dlg_Dn::Dlg_Dn(progress_taskbar& taskBar)
	: _taskBar(taskBar)
{
	setup.dialogId = DLG_PROGRESS;
}

void Dlg_Dn::init_controls()
{
	center_on_parent();
	_enable_x_button(false);

	_label.be(hwnd(), LBL_LBL);
	_progBar.be(hwnd(), PRO_PRO)
		.set_range(0, 100)
		.set_pos(0);
}

bool Dlg_Dn::show_err_and_close(const wchar_t* msg, const wstring& err)
{
	// Intended to be used form within a separate thread.
	ui_thread([&]() {
		_taskBar.set_error(true);
		sysdlg::msgbox(hwnd(), msg, err, MB_ICONERROR);
		_taskBar.clear();
		EndDialog(hwnd(), IDCANCEL);
	});
	return false;
}

void Dlg_Dn::_enable_x_button(bool enable) const
{
	// Enable/disable the X button to close the window; has no effect on Alt+F4.
	HMENU hMenu = GetSystemMenu(hwnd(), FALSE);
	if (hMenu) {
		UINT dwExtra = enable ? MF_ENABLED : (MF_DISABLED | MF_GRAYED);
		EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | dwExtra);
	}
}