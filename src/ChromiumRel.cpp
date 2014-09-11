
#include "ChromiumRel.h"

bool ChromiumRel::append(Xml& data)
{
	Xml::Node& root = data.root;

	if(!root.name.equals(L"ListBucketResult", String::Case::SENS)) return false;

	if(!root.children[0].name.equals(L"Name", String::Case::SENS) ||
		!root.children[0].value.equals(L"chromium-browser-continuous", String::Case::SENS)) return false;

	String *isTruncated = &root.firstChildByName(L"IsTruncated")->value;
	if(!isTruncated->equals(L"true", String::Case::SENS) &&
		!isTruncated->equals(L"false", String::Case::SENS)) return false;

	if(!this->_parseMorePrefixes(root)) return false;

	if(isTruncated->equals(L"true", String::Case::SENS)) { // more to come
		_nextMarker = root.firstChildByName(L"NextMarker")->value; // eg.: "Win/93883/"
	} else { // finished loading last piece of list
		_nextMarker = L"";
		_isFinished = true;
		_markers.sort([](const String& a, const String& b)->int {
			String s1, s2;
			s1.copyFrom(a.str() + 4, a.len() - 5); // number from "Win/93883/"
			s2.copyFrom(b.str() + 4, b.len() - 5);

			int i1 = s1.toInt(),
				i2 = s2.toInt();

			if(i1 < i2) return -1;
			else if(i1 == i2) return 0;
			return 1;
		});
	}

	return true;
}

bool ChromiumRel::_parseMorePrefixes(Xml::Node& root)
{
	Array<Xml::Node*> commonPrefixes = root.getChildrenByName(L"CommonPrefixes");
	
	int prevsz = _markers.size();
	_markers.realloc(prevsz + commonPrefixes.size()); // make room for more

	for(int i = 0; i < commonPrefixes.size(); ++i) {
		Xml::Node *prefix = &commonPrefixes[i]->children[0];
		_markers[prevsz + i] = prefix->value; // append to array, eg.: "Win/93883/"
	}

	return true;
}