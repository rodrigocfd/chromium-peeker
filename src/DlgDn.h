
#pragma once
#include "../c4w/c4w.h"
#include "../res/resource.h"
using namespace c4w;
using std::wstring;

// Base class to all download classes.
class DlgDn : public DialogModal {
protected:
	Window      label;
	ProgressBar progBar;
public:
	DlgDn() : DialogModal(DLG_PROGRESS) { }
	virtual ~DlgDn() = 0;
protected:
	void initCtrls();
	bool doShowErrAndClose(const wchar_t *msg, const wstring& err);
};