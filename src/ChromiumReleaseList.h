
#pragma once
#include "../toolow/Xml.h"

class ChromiumReleaseList {
public:
	ChromiumReleaseList() : _isFinished(false) { }
	
	bool                 append(Xml *data);
	bool                 isFinished() const { return _isFinished; }
	const wchar_t*       nextMarker() const { return _nextMarker.str(); }
	const Array<String>* markers() const    { return &_markers; }
	void                 reset()            { _markers.free(); _nextMarker = L""; _isFinished = false; }

private:
	Array<String> _markers;
	String        _nextMarker;
	bool          _isFinished;

	bool _parseMorePrefixes(Xml::Node *root);
};