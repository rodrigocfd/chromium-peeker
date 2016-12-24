
#pragma once
#include "../wet/dialog_main.h"
#include "../wet/download.h"
#include "../wet/label.h"
#include "../wet/listview.h"
#include "../wet/resizer.h"
#include "../wet/progress_taskbar.h"
#include "Chromium_Rel.h"

class Dlg_Main final : public wet::dialog_main {
private:
	Chromium_Rel           _chromiumRel;
	wet::progress_taskbar  _taskBar;
	wet::listview          _listview;
	wet::label             _lblLoaded;
	wet::resizer           _resizer;
	wet::download::session _session;

	INT_PTR proc(wet::params p) override;

public:
	Dlg_Main();
};