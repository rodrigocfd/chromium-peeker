//
// Basic array automation.
// Part of TOOLOW - Thin Object Oriented Layer Over Win32.
// @author Rodrigo Cesar de Freitas Dias
// @see https://github.com/rodrigocfd/toolow
//

#pragma once
#include <functional>
#include <new>
#include <stdlib.h>
#include <string.h>
using std::function;
using std::initializer_list; // because these should be keywords
#define MOVE std::move

template<typename T> class Array final {
private:
	T  *_ptr;
	int _sz;
public:
	Array()                        : _ptr(nullptr), _sz(0) { }
	Array(const Array& other)      : _ptr(nullptr), _sz(0) { operator=(other); }
	Array(Array&& other)           : _ptr(nullptr), _sz(0) { operator=(MOVE(other)); }
	Array(initializer_list<T> arr) : _ptr(nullptr), _sz(0) { operator=(arr); }
	explicit Array(int length)     : _ptr(nullptr), _sz(0) { this->realloc(length); }
	~Array()                       { this->free(); }

	Array& realloc(int length) {
		if(!length) return this->free();
		for(int i = length; i < _sz; ++i) _ptr[i].~T(); // when size < _sz, call destructors
		_ptr = (T*)::realloc(_ptr, sizeof(T) * length);
		for(int i = _sz; i < length; ++i) new(_ptr + i) T; // when size > _sz, call constructors
		_sz = length;
		return *this;
	}

	Array& free() {
		if(!_sz) return *this;
		for(int i = 0; i < _sz; ++i) _ptr[i].~T();
		::free(_ptr);
		_ptr = nullptr; _sz = 0;
		return *this;
	}

	Array& operator=(const Array& other) {
		this->realloc(other._sz);
		for(int i = 0; i < other._sz; ++i)
			_ptr[i] = other._ptr[i]; // deep copy, call operator=() on each element
		return *this;
	}
	Array& operator=(Array&& other) {
		this->free();
		_ptr = other._ptr; _sz = other._sz; // steal pointer
		other._ptr = nullptr; other._sz = 0;
		return *this;
	}
	Array& operator=(initializer_list<T> arr) {
		this->realloc((int)arr.size());
		for(int i = 0, sz = (int)arr.size(); i < sz; ++i)
			_ptr[i] = *(arr.begin() + i); // thanks to C++11 horrible design, this thing is necessary
		return *this;
	}

	int      size() const                { return _sz; }
	const T& operator[](int index) const { return _ptr[index]; }
	T&       operator[](int index)       { return _ptr[index]; }
	const T& last(int revIndex=0) const  { return _ptr[_sz - revIndex - 1]; }
	T&       last(int revIndex=0)        { return _ptr[_sz - revIndex - 1]; }

	Array& remove(int index) {
		if(index > _sz - 1) return *this; // index out of bounds
		if(_sz == 1) return this->free();
		_ptr[index].~T();
		if(index < _sz - 1)
			::memmove(_ptr + index, _ptr + index + 1, (_sz - index - 1) * sizeof(T));
		_ptr = (T*)::realloc(_ptr, sizeof(T) * --_sz);
		return *this;
	}

	Array& insert(int atIndex, const T *arr, int howMany) {
		if(atIndex > _sz) atIndex = _sz;
		_ptr = (T*)::realloc(_ptr, sizeof(T) * (_sz + howMany));
		if(atIndex < _sz)
			::memmove(_ptr + atIndex + howMany, _ptr + atIndex, (_sz - atIndex) * sizeof(T));
		for(int i = 0; i < howMany; ++i)
			new(_ptr + atIndex + i) T(arr[i]); // call copy constructor
		_sz += howMany;
		return *this;
	}
	Array& insert(int atIndex, initializer_list<T> arr) { return this->insert(atIndex, arr.begin(), (int)arr.size()); }
	Array& insert(int atIndex, const Array<T> *other)   { return this->insert(atIndex, other->_ptr, other->_sz); }
	Array& insert(int atIndex, const T& obj)            { return this->insert(atIndex, &obj, 1); }

	Array& append(const T *arr, int howMany) { return this->insert(_sz, arr, howMany); }
	Array& append(initializer_list<T> arr)   { return this->append(arr.begin(), (int)arr.size()); }
	Array& append(const Array<T> *other)     { return this->append(other->_ptr, other->_sz); }
	Array& append(const T& obj)              { return this->append(&obj, 1); }

	Array& reorder(int index, int newIndex) {
		if(index >= _sz || newIndex >= _sz) return *this;
		T *tmp = (T*)::_alloca(sizeof(T));
		::memcpy(tmp, _ptr + index, sizeof(T)); // store element to be moved
		newIndex > index ?
			::memmove(_ptr + index, _ptr + index + 1, sizeof(T) * (newIndex - index)) :
			::memmove(_ptr + newIndex + 1, _ptr + newIndex, sizeof(T) * (index - newIndex));
		::memcpy(_ptr + newIndex, tmp, sizeof(T));
		return *this;
	}

	Array& swap(Array& other) {
		T *tmpPtr = _ptr;    int tmpSz = _sz;
		_ptr = other._ptr;   _sz = other._sz;
		other._ptr = tmpPtr; other._sz = tmpSz;
		return *this;
	}

	void each(function<void(int i, T& elem)> callback) {
		// Example usage:
		// Array<int> nums;
		// nums.each([](int i, int& elem) { elem += 10; });
		for(int i = 0; i < _sz; ++i)
			callback(i, _ptr[i]);
	}
	void each(function<void(int i, const T& elem)> callback) const {
		// Example usage:
		// Array<int> nums;
		// nums.each([](int i, const int& elem) { int x = elem; });
		for(int i = 0; i < _sz; ++i)
			callback(i, _ptr[i]);
	}

	template<typename Ty> Array<Ty> transform(function<Ty(int i, const T& elem)> callback) {
		// Example usage:
		// Array<int> nums;
		// Array<float> trans = nums.transform<float>([](int i, const int& elem)->float { return (float)elem; });
		Array<Ty> ret(_sz); // prealloc
		for(int i = 0; i < _sz; ++i)
			ret[i] = callback(i, _ptr[i]); // invokes operator= on elements
		return ret;
	}

	Array filter(function<bool(int i, const T& elem)> callback) {
		// Example usage:
		// Array<float> nums;
		// Array<float> filtered = nums.filter([](int i, const float& elem)->bool { return elem < 25; });
		Array ret;
		for(int i = 0; i < _sz; ++i)
			if(callback(i, _ptr[i]))
				ret.append(_ptr[i]);
		return ret;
	}

	Array& sort(function<int(const T& a, const T& b)> callback) {
		// Example usage:
		// Array<float> nums;
		// nums.sort([](const float& a, const float& b)->int { return (int)(a - b); }); // lowest to highest
		::qsort_s(_ptr, _sz, sizeof(T), [](void *compareFunc, const void *a, const void *b)->int {
			typedef function<int(const T& a, const T& b)> Fun;
			Fun *callback = (Fun*)compareFunc;
			return (*callback)(*((const T*)a), *((const T*)b));
		}, &callback);
		return *this;
	}
};