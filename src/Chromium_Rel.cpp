
#include <algorithm>
#include <Windows.h>
#include "Chromium_Rel.h"
#include "../wet/str.h"
using namespace wet;
using std::vector;
using std::wstring;

bool Chromium_Rel::append(xml& data)
{
	xml::node& root = data.root;
	if (root.name != L"ListBucketResult") return false;

	if (root.children[0].name != L"Name" ||
		root.children[0].value != L"chromium-browser-continuous") return false;

	const wstring& isTruncated = root.first_child_by_name(L"IsTruncated")->value;
	if (isTruncated != L"true" &&
		isTruncated != L"false") return false;

	if (!this->_parse_more_prefixes(root)) return false;

	if (isTruncated == L"true") { // more to come
		_nextMarker = root.first_child_by_name(L"NextMarker")->value; // eg.: "Win/93883/"
	} else { // finished loading last piece of list
		_nextMarker = L"";
		_isFinished = true;
		std::sort(_markers.begin(), _markers.end(),
			[](const wstring& a, const wstring& b)->bool {
				wstring s1 = a.substr(4, a.length() - 5),
					s2 = b.substr(4, b.length() - 5); // number from "Win/93883/"
				return std::stoi(s1) < std::stoi(s2);
			});
	}

	return true;
}

void Chromium_Rel::reset()
{
	_markers.resize(0);
	_nextMarker = L"";
	_isFinished = false;
}

bool Chromium_Rel::_parse_more_prefixes(xml::node& root)
{
	vector<xml::node*> commonPrefixes = root.children_by_name(L"CommonPrefixes");
	size_t prevsz = _markers.size();
	_markers.reserve(prevsz + commonPrefixes.size()); // make room for more

	for (xml::node *cp : commonPrefixes) {
		xml::node *prefix = &cp->children[0];
		_markers.emplace_back(prefix->value); // eg.: "Win/93883/"
	}
	return true;
}