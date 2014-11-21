//
// Associative array automation.
// Part of TOOLOW - Thin Object Oriented Layer Over Win32.
// @author Rodrigo Cesar de Freitas Dias
// @see https://github.com/rodrigocfd/toolow
//

#pragma once
#include "String.h"

template<typename T> class Hash final {
public:
	struct Elem final {
		Elem()                             { }
		Elem(const Elem& other)            : key(other.key), val(other.val) { }
		Elem(Elem&& other)                 : key(MOVE(other.key)), val(MOVE(val)) { }
		explicit Elem(const wchar_t *k)    : key(k) { }
		Elem& operator=(const Elem& other) { key = other.key; val = other.val; return *this; }
		Elem& operator=(Elem&& other)      { key = MOVE(other.key); val = MOVE(other.val); return *this; }
		String key;
		T      val;
	};

private:
	Array<Elem> _elems;
	int         _szUsed;
public:
	Hash()                  : _szUsed(0) { }
	Hash(const Hash& other) : _szUsed(0) { operator=(other); }
	Hash(Hash&& other)      : _szUsed(0) { operator=(MOVE(other)); }

	int         size() const                     { return _szUsed; }
	bool        exists(const wchar_t *key) const { return this->_byKey(key) > -1; }
	const Elem* at(int index) const              { return &_elems[index]; }
	Elem*       at(int index)                    { return &_elems[index]; }

	const T& operator[](const String& key) const  { return operator[](key.str()); }
	const T& operator[](const wchar_t *key) const {
		int idx = this->_byKey(key);
		if(idx == -1) // key not found
			return _elems[0].val; // not right... lame C++ won't allow null references!
		return _elems[idx].val;
	}
	T& operator[](const String& key)  { return operator[](key.str()); }
	T& operator[](const wchar_t *key) {
		int idx = this->_byKey(key);
		if(idx == -1) { // key not found
			this->reserve(++_szUsed); // so let's insert it
			_elems[_szUsed - 1] = Elem(key); // create entry with default constructor
			idx = _szUsed - 1;
		}
		return _elems[idx].val;
	}

	Hash& reserve(int howMany) {
		if(howMany > _elems.size())
			_elems.realloc(howMany); // always grows
		return *this;
	}

	Hash& operator=(const Hash& other) {
		this->reserve(other._szUsed);
		for(int i = 0; i < other._szUsed; ++i) {
			_elems[i].key = other._elems[i].key;
			_elems[i].val = other._elems[i].val;
		}
		_szUsed = other._szUsed;
		return *this;
	}
	Hash& operator=(Hash&& other) {
		_elems = MOVE(other._elems);
		_szUsed = other._szUsed;
		return *this;
	}

	Hash& removeAll()                { _elems.free(); _szUsed = 0; return *this; }
	Hash& remove(const wchar_t *key) { return this->remove(this->_byKey(key)); }
	Hash& remove(int index) {
		if(index >= 0 && index <= _szUsed - 1) {
			_elems.remove(index);
			--_szUsed;
		}
		return *this;
	}

	void each(function<void(Elem& elem)> callback) {
		// Example usage:
		// Hash<int> nums;
		// nums.each([](Hash<int>::Elem& elem) { elem.val += 10; });
		for(int i = 0; i < _szUsed; ++i)
			callback(_elems[i]);
	}
	void each(function<void(const Elem& elem)> callback) const {
		// Example usage:
		// Hash<int> nums;
		// nums.each([](const Hash<int>::Elem& elem) { int x = elem.val; });
		for(int i = 0; i < _szUsed; ++i)
			callback(_elems[i]);
	}
private:
	int _byKey(const wchar_t *keyName) const {
		for(int i = 0; i < _szUsed; ++i) // linear search
			if(_elems[i].key.equalsCS(keyName)) // an empty string is also a valid key
				return i;
		return -1; // not found
	}
};