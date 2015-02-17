
#include "DlgDn.h"

DlgDn::~DlgDn()
{
}

void DlgDn::initCtrls()
{
	// Call this method at the beginning of WM_INITDIALOG processing.

	this->setXButton(false);
	this->label = this->getChild(LBL_LBL);

	( this->progBar = this->getChild(PRO_PRO) )
		.setRange(0, 100)
		.setPos(0);
}

bool DlgDn::doShowErrAndClose(const wchar_t *msg, const wstring& err)
{
	// Intended to be used form within a separate thread.
	this->sendFunction([&]() {
		this->messageBox(msg, err, MB_ICONERROR);
		this->endDialog(IDCANCEL);
	});
	return false;
}