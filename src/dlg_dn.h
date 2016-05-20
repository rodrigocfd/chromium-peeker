
#pragma once
#include "../winlamb/dialog_modal.h"
#include "../winlamb/msg_thread.h"
#include "../winutil/label.h"
#include "../winutil/progressbar.h"
#include "../winutil/taskbar_progress.h"

// Base class to all download classes.
class dlg_dn : public winlamb::dialog_modal,
	public winlamb::dialog_msg_thread
{
private:
	func_msg_type _userInitDialog;
protected:
	winutil::label             _label;
	winutil::progressbar       _progBar;
	winutil::taskbar_progress& _taskBar;
public:
	virtual ~dlg_dn() = 0;
	dlg_dn(winutil::taskbar_progress& taskBar);
protected:
	void init_controls();
	bool show_err_and_close(const wchar_t* msg, const std::wstring& err);
};