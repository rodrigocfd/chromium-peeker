
#pragma once
#include "../winlamb/dialog_main.h"
#include "../winlamb/msg_command.h"
#include "../winlamb/msg_initmenupopup.h"
#include "../winutil/Internet.h"
#include "../winutil/Label.h"
#include "../winutil/ListView.h"
#include "../winutil/Resizer.h"
#include "../winutil/TaskBarProgress.h"
#include "ChromiumRel.h"

class FrmMain final : public winlamb::dialog_main,
	public winlamb::dialog_msg_command,
	public winlamb::dialog_msg_initmenupopup
{
private:
	ChromiumRel     _chromiumRel;
	TaskBarProgress _taskBar;
	ListView        _listview;
	Label           _lblLoaded;
	Resizer         _resizer;
	InternetSession _session;
public:
	FrmMain();
};