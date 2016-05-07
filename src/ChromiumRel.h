
#pragma once
#include "../winutil/xml.h"

class ChromiumRel final {
private:
	std::vector<std::wstring> _markers;
	std::wstring              _nextMarker;
	bool                      _isFinished;
public:
	ChromiumRel() : _isFinished(false) { }
	
	bool           append(winutil::xml& data);
	bool           isFinished() const                { return _isFinished; }
	const wchar_t* nextMarker() const                { return _nextMarker.c_str(); }
	const std::vector<std::wstring>& markers() const { return _markers; }
	void           reset();
private:
	bool _parseMorePrefixes(winutil::xml::node& root);
};