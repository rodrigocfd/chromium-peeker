
#pragma once
#include "../wet/dialog_modal.h"
#include "../wet/label.h"
#include "../wet/progressbar.h"
#include "../wet/progress_taskbar.h"

// Base class to all download classes.
class Dlg_Dn : public wet::dialog_modal {
protected:
	wet::label             _label;
	wet::progressbar       _progBar;
	wet::progress_taskbar& _taskBar;

public:
	virtual ~Dlg_Dn() = 0;
	Dlg_Dn(wet::progress_taskbar& taskBar);

protected:
	void init_controls();
	bool show_err_and_close(const wchar_t* msg, const std::wstring& err);
};