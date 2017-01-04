
#pragma once
#include "../winlamb/dialog_main.h"
#include "../winlamb/download.h"
#include "../winlamb/label.h"
#include "../winlamb/listview.h"
#include "../winlamb/resizer.h"
#include "../winlamb/progress_taskbar.h"
#include "Chromium_Rel.h"

class Dlg_Main final : public wl::dialog_main {
private:
	Chromium_Rel          _chromiumRel;
	wl::progress_taskbar  _taskBar;
	wl::listview          _listview;
	wl::label             _lblLoaded;
	wl::resizer           _resizer;
	wl::download::session _session;

public:
	Dlg_Main();
};