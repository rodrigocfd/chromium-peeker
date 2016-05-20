
#pragma once
#include "../winutil/xml.h"

class chromium_rel final {
private:
	std::vector<std::wstring> _markers;
	std::wstring              _nextMarker;
	bool                      _isFinished;
public:
	chromium_rel() : _isFinished(false) { }
	
	bool           append(winutil::xml& data);
	bool           is_finished() const               { return _isFinished; }
	const wchar_t* next_marker() const               { return _nextMarker.c_str(); }
	const std::vector<std::wstring>& markers() const { return _markers; }
	void           reset();
private:
	bool _parse_more_prefixes(winutil::xml::node& root);
};