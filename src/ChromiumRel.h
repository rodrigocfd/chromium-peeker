
#pragma once
#include "../wolf/wolf.h"

class ChromiumRel final {
private:
	std::vector<std::wstring> _markers;
	std::wstring _nextMarker;
	bool _isFinished;
public:
	ChromiumRel() : _isFinished(false) { }
	
	bool           append(wolf::file::Xml& data);
	bool           isFinished() const { return _isFinished; }
	const wchar_t* nextMarker() const { return _nextMarker.c_str(); }
	const std::vector<std::wstring>& markers() const { return _markers; }
	void           reset()            { _markers.resize(0); _nextMarker = L""; _isFinished = false; }
private:
	bool _parseMorePrefixes(wolf::file::Xml::Node& root);
};