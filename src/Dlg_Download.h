
#pragma once
#include "std.h"
#include <winlamb/dialog_modal.h>
#include <winlamb/label.h>
#include <winlamb/progressbar.h>
#include <winlamb/progress_taskbar.h>

#define BASE_URL L"http://commondatastorage.googleapis.com/chromium-browser-continuous"
#define REFERRER BASE_URL L"/index.html?path=Win/"

// Base class to all download classes.
class Dlg_Download : public wl::dialog_modal {
protected:
	wl::label             m_lblTitle;
	wl::progressbar       m_progBar;
	wl::progress_taskbar& m_taskbarProg;

public:
	virtual ~Dlg_Download() = 0;
	Dlg_Download(wl::progress_taskbar& tb);

protected:
	void init_controls();
	void handle_close_msg();
	void show_err_and_close(const wchar_t* msg, const wstring& err);
};