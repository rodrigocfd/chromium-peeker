
#pragma once
#include "../wolf/wolf.h"
#include "ChromiumRel.h"

class FrmMain final : public wolf::WindowMain {
private:
	ChromiumRel           _chromiumRel;
	wolf::TaskBarProgress _taskBar;
	wolf::ListView        _listview;
	Window                _lblLoaded;
	wolf::Resizer         _resz;
	wolf::InternetSession _session;
public:
	FrmMain();
};