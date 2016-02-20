
#pragma once
#include "../winlamb/dialog_modal.h"
#include "../winlamb/msg_thread.h"
#include "../winutil/Label.h"
#include "../winutil/ProgressBar.h"
#include "../winutil/TaskBarProgress.h"

// Base class to all download classes.
class FrmDn : public winlamb::dialog_modal,
	public winlamb::msg_thread_dialog
{
private:
	msg_func_type _userInitDialog;
protected:
	Label            _label;
	ProgressBar      _progBar;
	TaskBarProgress& _taskBar;
public:
	virtual ~FrmDn() = 0;
	FrmDn(TaskBarProgress& taskBar);
protected:
	void initControls();
	bool doShowErrAndClose(const wchar_t *msg, const std::wstring& err);
};