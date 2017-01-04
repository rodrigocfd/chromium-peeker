
#pragma once
#include "../winlamb/dialog_modal.h"
#include "../winlamb/label.h"
#include "../winlamb/progressbar.h"
#include "../winlamb/progress_taskbar.h"

// Base class to all download classes.
class Dlg_Dn : public wl::dialog_modal {
protected:
	wl::label             _label;
	wl::progressbar       _progBar;
	wl::progress_taskbar& _taskBar;

public:
	virtual ~Dlg_Dn() = 0;
	Dlg_Dn(wl::progress_taskbar& taskBar);

protected:
	void init_controls();
	bool show_err_and_close(const wchar_t* msg, const std::wstring& err);

private:
	void _enable_x_button(bool enable) const;
};