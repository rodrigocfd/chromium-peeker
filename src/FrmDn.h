
#pragma once
#include "../winlamb/window_dialog_modal.h"
#include "../winutil/Label.h"
#include "../winutil/ProgressBar.h"
#include "../winutil/TaskBarProgress.h"

// Base class to all download classes.
class FrmDn : public winlamb::window_dialog_modal {
private:
	msg_func_type _userInitDialog;
protected:
	Label            _label;
	ProgressBar      _progBar;
	TaskBarProgress& _taskBar;
public:
	virtual ~FrmDn() = 0;
	FrmDn(TaskBarProgress& taskBar);
	virtual void on_message(UINT msg, msg_func_type callback) override;
protected:
	bool doShowErrAndClose(const wchar_t *msg, const std::wstring& err);
};