
#pragma once
#include "../wolf/wolf.h"
#include "ChromiumRel.h"
#include "../res/resource.h"

class DlgMain final : public wolf::wnd::DialogMain {
public:
	DlgMain() : DialogMain(DLG_MAIN, ICO_CHROMIUM) { }
private:
	void events() override;

	ChromiumRel          _chromiumRel;
	wolf::ctrl::ListView _listview;
	wolf::wnd::Wnd       _lblLoaded;
	wolf::ctrl::Resizer  _resz;
	wolf::net::Session   _session;
};