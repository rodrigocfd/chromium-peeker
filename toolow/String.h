//
// Ordinary string implementation, ever growing, with buffer capabilities.
// Part of TOOLOW - Thin Object Oriented Layer Over Win32.
// @author Rodrigo Cesar de Freitas Dias
// @see https://github.com/rodrigocfd/toolow
//

#pragma once
#include "Array.h"
#include <Windows.h>

class String;
void dbg(const wchar_t *fmt, ...);
void dbg(const String& s);

class String final {
private:
	Array<wchar_t> _arr;
public:
	friend void dbg(const wchar_t*, ...);
	enum class Case { SENS, INSENS };
	enum class Diacritics { KEEP, REM };

	String()                    { }
	String(const wchar_t *s)    { operator=(s); }
	String(const String& other) : _arr(other._arr) { }
	String(String&& other)      : _arr(MOVE(other._arr)) { }

	int            len() const                     { return _arr.size() ? ::lstrlen(&_arr[0]) : 0; }
	const wchar_t* str() const                     { return _arr.size() ? &_arr[0] : L""; }
	bool           isEmpty() const                 { return !_arr.size() || _arr[0] == L'\0'; }
	const wchar_t& operator[](int index) const     { return _arr[index]; }
	wchar_t&       operator[](int index)           { return _arr[index]; }
	const wchar_t* ptrAt(int index) const          { return &_arr[index]; }
	wchar_t*       ptrAt(int index)                { return &_arr[index]; }
	String&        operator=(const String& other)  { return operator=(other.str()); }
	String&        operator=(String&& other)       { _arr = MOVE(other._arr); return *this; }
	String&        operator=(const wchar_t *s);
	String&        reserve(int numCharsWithoutNull);
	int            reserved() const                { return _arr.size() - 1; }
	String&        append(const String& s)         { return append(s.str()); }
	String&        append(const wchar_t *s);
	String&        append(initializer_list<const wchar_t*> arr);
	String&        append(wchar_t ch);
	String&        insert(int at, const String& s) { return insert(at, s.str()); }
	String&        insert(int at, const wchar_t *s);
	String         substr(int start) const;
	String         substr(int start, int length) const;
	String&        copyFrom(const wchar_t *src, int numChars);
	String&        appendFrom(const wchar_t *src, int numChars);
	String&        trim();
	String&        removeDiacritics()                                                       { if(!this->isEmpty()) String::_RemDiacr(&_arr[0]); return *this; }
	bool           equals(const wchar_t *s, Case c, Diacritics d=Diacritics::KEEP) const    { return !String::LexicalCompare(this->str(), s, c, d); }
	bool           equals(const String& other, Case c, Diacritics d=Diacritics::KEEP) const { return !String::LexicalCompare(this->str(), other.str(), c, d); }
	bool           beginsWith(const wchar_t *s, Case c, Diacritics d=Diacritics::KEEP) const;
	bool           endsWith(const wchar_t *s, Case c, Diacritics d=Diacritics::KEEP) const;
	bool           endsWith(wchar_t ch) const;
	bool           isInt() const;
	bool           isFloat() const;
	int            toInt() const   { return this->isInt() ? ::_wtoi(this->str()) : 0; }
	double         toFloat() const { return this->isFloat() ? ::_wtof(this->str()) : 0; }
	int            find(wchar_t ch) const;
	int            find(const wchar_t *substring, Case c, Diacritics d=Diacritics::KEEP) const;
	int            findr(wchar_t ch) const;
	String&        replace(const wchar_t *target, const wchar_t *replacement, Case c, Diacritics d=Diacritics::KEEP);
	String&        invert();
	Array<String>  explode(const wchar_t *delimiters) const;

	static String        Fmt(const wchar_t *fmt, ...);
	static String        ParseUtf8(const BYTE *data, int length);
	static String        ParseUtf8(const Array<BYTE>& data) { return data.size() ? ParseUtf8(&data[0], data.size()) : L""; }
	static Array<String> ExplodeQuoted(const wchar_t *quotedStr);
	static Array<String> ExplodeMulti(const wchar_t *multiStr);
	static int           LexicalCompare(const wchar_t *a, const wchar_t *b, Case c, Diacritics d=Diacritics::KEEP);
	static int           LexicalCompare(const String& a, const String& b, Case c, Diacritics d=Diacritics::KEEP) { return String::LexicalCompare(a.str(), b.str(), c, d); }
	inline static void   CaesarCipher(String& s, int shift) { for(int i = 0, len = s.len(); i < len; ++i) s[i] += shift; }
	inline static bool   CharIsWithin(int x, int a, int b)  { return x >= a && x <= b; }
	inline static bool   CharIsBetween(int x, int a, int b) { return x > a && x < b; }
private:
	static String _Formatv(const wchar_t *fmt, va_list args);
	static void   _RemDiacr(wchar_t *txt);
};