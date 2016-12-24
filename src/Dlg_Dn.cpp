
#include "Dlg_Dn.h"
#include "../wet/sysdlg.h"
#include "../res/resource.h"
using namespace wet;
using std::wstring;

Dlg_Dn::~Dlg_Dn() {
}

Dlg_Dn::Dlg_Dn(progress_taskbar& taskBar)
	: _taskBar(taskBar) {
	setup.dialogId = DLG_PROGRESS;
}

void Dlg_Dn::init_controls() {
	center_on_parent();
	enable_x_button(false);

	_label.be(this, LBL_LBL);
	_progBar.be(this, PRO_PRO)
		.set_range(0, 100)
		.set_pos(0);
}

bool Dlg_Dn::show_err_and_close(const wchar_t* msg, const wstring& err) {
	// Intended to be used form within a separate thread.
	ui_thread([&]() {
		_taskBar.set_error(true);
		sysdlg::msgbox(this, msg, err, MB_ICONERROR);
		_taskBar.clear();
		EndDialog(hwnd(), IDCANCEL);
	});
	return false;
}