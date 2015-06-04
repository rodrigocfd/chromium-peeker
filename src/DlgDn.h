
#pragma once
#include "../wolf/wolf.h"
#include "../res/resource.h"

// Base class to all download classes.
class DlgDn : public wolf::DialogModal {
public:
	DlgDn() : DialogModal(DLG_PROGRESS) { }
	virtual ~DlgDn() = 0;
protected:
	void _initCtrls();
	bool _doShowErrAndClose(const wchar_t *msg, const std::wstring& err);
	
	wolf::Window _label;
	wolf::ctrl::ProgressBar _progBar;
};