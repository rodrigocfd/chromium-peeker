
#pragma once
#include "../winlamb/dialog_main.h"
#include "../winlamb/msg_command.h"
#include "../winlamb/msg_initmenupopup.h"
#include "../winlamb/download.h"
#include "../winlamb/label.h"
#include "../winlamb/listview.h"
#include "../winlamb/resizer.h"
#include "../winlamb/progress_taskbar.h"
#include "Chromium_Rel.h"

class Dlg_Main final :
	public    wl::dialog_main,
	protected wl::msg_command,
	protected wl::msg_initmenupopup
{
private:
	Chromium_Rel          m_chromiumRel;
	wl::progress_taskbar  m_taskbarProgr;
	wl::listview          m_lstEntries;
	wl::label             m_lblLoaded;
	wl::resizer           m_resz;
	wl::download::session m_session;

public:
	Dlg_Main();
};