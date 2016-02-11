
#pragma once
#include "../winlamb/dialog_main.h"
#include "../winutil/Internet.h"
#include "../winutil/Label.h"
#include "../winutil/ListView.h"
#include "../winutil/Resizer.h"
#include "../winutil/TaskBarProgress.h"
#include "ChromiumRel.h"

class FrmMain final : public winlamb::dialog_main {
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