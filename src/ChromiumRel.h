
#pragma once
#include "../toolow/toolow.h"

class ChromiumRel final {
private:
	Array<String> _markers;
	String        _nextMarker;
	bool          _isFinished;
public:
	ChromiumRel() : _isFinished(false) { }
	
	bool                 append(Xml& data);
	bool                 isFinished() const { return _isFinished; }
	const wchar_t*       nextMarker() const { return _nextMarker.str(); }
	const Array<String>& markers() const    { return _markers; }
	void                 reset()            { _markers.resize(0); _nextMarker = L""; _isFinished = false; }
private:
	bool _parseMorePrefixes(Xml::Node& root);
};