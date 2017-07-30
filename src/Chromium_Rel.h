
#pragma once
#include <winlamb-more/xml.h>

class Chromium_Rel final {
private:
	std::vector<std::wstring> _markers;
	std::wstring              _nextMarker;
	bool                      _isFinished;
public:
	Chromium_Rel() : _isFinished(false) { }
	
	bool           append(wl::xml& data);
	bool           is_finished() const               { return _isFinished; }
	const wchar_t* next_marker() const               { return _nextMarker.c_str(); }
	const std::vector<std::wstring>& markers() const { return _markers; }
	void           reset();
private:
	bool _parse_more_prefixes(wl::xml::node& root);
};