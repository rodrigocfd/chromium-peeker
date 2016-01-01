
#pragma once
#include "../wolf/wolf.h"

// Base class to all download classes.
class FrmDn : public wolf::WindowModal {
protected:
	wolf::Window          _label;
	wolf::ProgressBar     _progBar;
	wolf::TaskBarProgress _taskBar;
public:
	virtual ~FrmDn() = 0;
	FrmDn(wolf::WindowMain *wmain);
protected:
	bool _doShowErrAndClose(const wchar_t *msg, const std::wstring& err);
};