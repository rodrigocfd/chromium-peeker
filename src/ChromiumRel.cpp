
#include "ChromiumRel.h"
#include <algorithm>
using std::vector;
using std::wstring;

bool ChromiumRel::append(Xml& data)
{
	Xml::Node& root = data.root;

	if (!str::Cmp(str::Sens::YES, root.name, L"ListBucketResult")) return false;

	if (!str::Cmp(str::Sens::YES, root.children[0].name, L"Name") ||
		!str::Cmp(str::Sens::YES, root.children[0].value, L"chromium-browser-continuous")) return false;

	const wstring& isTruncated = root.firstChildByName(L"IsTruncated")->value;
	if (!str::Cmp(str::Sens::YES, isTruncated, L"true") &&
		!str::Cmp(str::Sens::YES, isTruncated, L"false")) return false;

	if (!this->_parseMorePrefixes(root)) return false;

	if (str::Cmp(str::Sens::YES, isTruncated, L"true")) { // more to come
		_nextMarker = root.firstChildByName(L"NextMarker")->value; // eg.: "Win/93883/"
	} else { // finished loading last piece of list
		_nextMarker = L"";
		_isFinished = true;
		std::sort(_markers.begin(), _markers.end(), [](const wstring& a, const wstring& b)->bool {
			wstring s1 = a.substr(4, a.length() - 5),
				s2 = b.substr(4, b.length() - 5); // number from "Win/93883/"
			return std::stoi(s1) < std::stoi(s2);
		});
	}

	return true;
}

bool ChromiumRel::_parseMorePrefixes(Xml::Node& root)
{
	vector<Xml::Node*> commonPrefixes = root.getChildrenByName(L"CommonPrefixes");
	
	size_t prevsz = _markers.size();
	_markers.reserve(prevsz + commonPrefixes.size()); // make room for more

	for (Xml::Node *cp : commonPrefixes) {
		Xml::Node *prefix = &cp->children[0];
		_markers.emplace_back(prefix->value); // eg.: "Win/93883/"
	}
	return true;
}