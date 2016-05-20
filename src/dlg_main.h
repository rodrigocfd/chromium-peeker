
#pragma once
#include "../winlamb/dialog_main.h"
#include "../winlamb/msg_command.h"
#include "../winlamb/msg_initmenupopup.h"
#include "../winutil/internet.h"
#include "../winutil/label.h"
#include "../winutil/listview.h"
#include "../winutil/resizer.h"
#include "../winutil/taskbar_progress.h"
#include "chromium_rel.h"

class dlg_main final : public winlamb::dialog_main,
	public winlamb::dialog_msg_command,
	public winlamb::dialog_msg_initmenupopup
{
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