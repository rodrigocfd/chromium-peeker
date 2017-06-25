
#pragma once
#include "../winlamb/dialog_modal.h"
#include "../winlamb/msg_ui_thread.h"
#include "../winlamb-more/label.h"
#include "../winlamb-more/progressbar.h"
#include "../winlamb-more/progress_taskbar.h"

#define BASE_URL L"http://commondatastorage.googleapis.com/chromium-browser-continuous"
#define REFERRER BASE_URL L"/index.html?path=Win/"

// Base class to all download classes.
class Dlg_Dn :
	public wl::dialog_modal,
	public wl::msg_ui_thread
{
protected:
	wl::label             m_lblTitle;
	wl::progressbar       m_progBar;
	wl::progress_taskbar& m_taskbarProg;
public:
	virtual ~Dlg_Dn() = 0;
	Dlg_Dn(wl::progress_taskbar& tb);

protected:
	void init_controls();
	void handle_close_msg();
	bool show_err_and_close(const wchar_t* msg, const std::wstring& err);
};