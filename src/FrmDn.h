
#pragma once
#include "../winlamb/dialog_modal.h"
#include "../winlamb/msg_thread.h"
#include "../winutil/label.h"
#include "../winutil/progressbar.h"
#include "../winutil/taskbar_progress.h"

// Base class to all download classes.
class FrmDn : public winlamb::dialog_modal,
	public winlamb::dialog_msg_thread
{
private:
	func_msg_type _userInitDialog;
protected:
	winutil::label             _label;
	winutil::progressbar       _progBar;
	winutil::taskbar_progress& _taskBar;
public:
	virtual ~FrmDn() = 0;
	FrmDn(winutil::taskbar_progress& taskBar);
protected:
	void initControls();
	bool doShowErrAndClose(const wchar_t *msg, const std::wstring& err);
};