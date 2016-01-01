
#pragma once
#include "FrmDn.h"

// Download the marker zip file to disk, destination chosen by user.
class FrmDnZip final : public FrmDn {
private:
	wolf::InternetSession& _session;
	std::wstring _marker, _dest;
public:
	FrmDnZip(wolf::WindowMain *wmain,
		wolf::InternetSession& session,
		const std::wstring& marker);
private:
	bool _doDownload();
	bool _doReceiveData(wolf::InternetDownload& zipdl, wolf::File& fout);
};