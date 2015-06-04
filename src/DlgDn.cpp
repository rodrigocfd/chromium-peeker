
#include "DlgDn.h"
using namespace wolf;
using std::wstring;

DlgDn::~DlgDn()
{
}

void DlgDn::_initCtrls()
{
	// Call this method at the beginning of WM_INITDIALOG processing.

	this->setXButton(false);
	_label = this->getChild(LBL_LBL);

	( _progBar = this->getChild(PRO_PRO) )
		.setRange(0, 100)
		.setPos(0);
}

bool DlgDn::_doShowErrAndClose(const wchar_t *msg, const wstring& err)
{
	// Intended to be used form within a separate thread.
	this->inOrigThread([&]() {
		this->messageBox(msg, err, MB_ICONERROR);
		this->endDialog(IDCANCEL);
	});
	return false;
}