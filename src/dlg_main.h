
#pragma once
#include "../winlamb/dialog_main.h"
#include "../winutil/internet.h"
#include "../winutil/label.h"
#include "../winutil/listview.h"
#include "../winutil/resizer.h"
#include "../winutil/taskbar_progress.h"
#include "chromium_rel.h"

class dlg_main final : public winlamb::dialog_main {
private:
	chromium_rel              _chromium_rel;
	winutil::taskbar_progress _taskBar;
	winutil::listview         _listview;
	winutil::label            _lblLoaded;
	winutil::resizer          _resizer;
	winutil::internet_session _session;
public:
	dlg_main();
};