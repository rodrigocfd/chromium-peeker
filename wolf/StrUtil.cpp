/*!
 * String utilities.
 * Part of WOLF - Win32 Object Lambda Framework.
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/wolf
 */

#include "StrUtil.h"
#include <algorithm>
#include <cwctype>
#pragma warning(disable:4996) // _vsnwprintf
using namespace wolf;
using std::vector;
using std::wstring;

static wchar_t _ChangeCase(wchar_t ch, bool toUpper)
{
	if (toUpper && (
		(ch >= L'a' && ch <= L'z') ||
		(ch >= L'à' && ch <= L'ö') ||
		(ch >= L'ø' && ch <= L'þ') ))
	{
		return ch - 32;
	}
	else if (!toUpper && (
		(ch >= L'A' && ch <= L'Z') ||
		(ch >= L'À' && ch <= L'Ö') ||
		(ch >= L'Ø' && ch <= L'Þ') ))
	{
		return ch + 32;
	}
	return ch;
}

wstring& wolf::ToUpper(wstring& s)
{
	for (wchar_t& ch : s)
		ch = _ChangeCase(ch, true);
	return s;
}

wstring& wolf::ToLower(wstring& s)
{
	for (wchar_t& ch : s)
		ch = _ChangeCase(ch, false);
	return s;
}

bool wolf::IsInt(const wstring& s)
{
	if (s.empty()) return false;

	for (const wchar_t& ch : s) {
		if (ch < L'0' || ch > L'9')
			return false;
	}
	return true;
}

bool wolf::IsFloat(const wstring& s)
{
	if (s.empty()) return false;

	bool dotFound = false;
	for (const wchar_t& ch : s) {
		if (ch == L'.') {
			if (dotFound) {
				return false;
			} else {
				dotFound = true; // allows only 1 dot character
			}
		} else if (ch < L'0' || ch > L'9') {
			return false;
		}
	}
	return true;
}


static int _Compare(const wchar_t *a, const wchar_t *b, bool isCS, size_t numCharsToSee)
{
	if (!a && !b) return 0;
	if (!a) return -1; else if (!b) return 1; // different strings

	int count = 0;
	for (;;) {
		if (!*a && !*b) return 0; // end of both strings reached

		if (isCS && *a != *b) {
			return static_cast<int>(*a - *b); // different strings
		} else if (!isCS) {
			wchar_t aa = _ChangeCase(*a, true), // cache uppercase
				bb = _ChangeCase(*b, true);
			if (aa != bb)
				return aa - bb; // different strings
		}

		++a; ++b; ++count;
		if (numCharsToSee && count == numCharsToSee) return 0;
	}
	return -42; // never happens
}

int wolf::CompareS(const wchar_t *a, const wchar_t *b, size_t nChars) { return _Compare(a, b, true, nChars); }
int wolf::CompareI(const wchar_t *a, const wchar_t *b, size_t nChars) { return _Compare(a, b, false, nChars); }

static bool _BeginsWith(const wstring& s, const wchar_t *what, bool isCS)
{
	if (s.empty()) return false;

	size_t whatLen = lstrlen(what);
	if (!whatLen || whatLen > s.length()) return false;

	return !_Compare(&s[0], what, isCS, whatLen);
}

bool wolf::BeginsWithS(const wstring& s, const wchar_t *what) { return _BeginsWith(s, what, true); }
bool wolf::BeginsWithI(const wstring& s, const wchar_t *what) { return _BeginsWith(s, what, false); }

static bool _EndsWith(const wstring& s, const wchar_t *what, bool isCS)
{
	if (s.empty()) return false;

	size_t whatLen = lstrlen(what);
	if (!whatLen || whatLen > s.length()) return false;

	return !_Compare(&s[s.length() - whatLen], what, isCS, whatLen);
}

bool wolf::EndsWithS(const wstring& s, const wchar_t *what) { return _EndsWith(s, what, true); }
bool wolf::EndsWithI(const wstring& s, const wchar_t *what) { return _EndsWith(s, what, false); }


static const wchar_t* _FindCh(const wchar_t *s, size_t slen, wchar_t whatCh, bool isCS, bool isReverse)
{
	wchar_t bb = isCS ? whatCh : _ChangeCase(whatCh, true);
	if (isReverse) {
		for (size_t i = slen; i-- > 0; ) {
			wchar_t aa = isCS ? s[i] : _ChangeCase(s[i], true);
			if (aa == bb)
				return s + i;
		}
	} else {
		for (size_t i = 0; i < slen; ++i) {
			wchar_t aa = isCS ? s[i] : _ChangeCase(s[i], true);
			if (aa == bb)
				return s + i;
		}
	}
	return nullptr; // not found
}
static int _FindCh(const wstring& s, size_t slen, wchar_t whatCh, bool isCS, bool isReverse)
{
	const wchar_t *f = _FindCh(s.c_str(), slen ? slen : s.length(), whatCh, isCS, isReverse);
	return f ? static_cast<int>(f - s.c_str()) : -1;
}

static const wchar_t* _FindStr(const wchar_t *s, size_t slen, const wchar_t *whatStr, bool isCS, bool isReverse)
{
	if (!slen) slen = lstrlen(s);
	int lenWhat = lstrlen(whatStr);
	if (lenWhat == 1) return _FindCh(s, slen, *whatStr, isCS, isReverse);

	wchar_t *tmpWhat = nullptr;
	if (!isCS) { // create temp with uppercase version of "what"
		tmpWhat = static_cast<wchar_t*>(_alloca((lenWhat + 1) * sizeof(wchar_t)));
		for (int i = 0; i < lenWhat; ++i)
			tmpWhat[i] = _ChangeCase(whatStr[i], true);
		tmpWhat[lenWhat] = L'\0'; // trailing null
	} else {
		tmpWhat = const_cast<wchar_t*>(whatStr);
	}

	if (isReverse) {
		for (size_t lenS = slen; ; ) {
			const wchar_t *found = _FindCh(s, lenS, *tmpWhat, isCS, true);
			if (!found) break;
			const wchar_t *pS = found + 1,
				*pWhat = tmpWhat + 1;
			while (*pS && (isCS ? *pS : _ChangeCase(*pS, true)) == *pWhat) {
				if (!*++pWhat) return found;
				++pS;
			}
			lenS = found - s;
		}
	} else {
		for (int i = 0; ; ) {
			const wchar_t *found = _FindCh(s + i, slen - i, *tmpWhat, isCS, false);
			if (!found) break;
			const wchar_t *pS = found + 1,
				*pWhat = tmpWhat + 1;
			while (*pS && (isCS ? *pS : _ChangeCase(*pS, true)) == *pWhat) {
				if (!*++pWhat) return found;
				++pS;
			}
			++i;
		}
	}
	return nullptr; // not found
}
static int _FindStr(const wstring& s, size_t slen, const wchar_t *whatStr, bool isCS, bool isReverse)
{
	const wchar_t *f = _FindStr(s.c_str(), slen ? slen : s.length(), whatStr, isCS, isReverse);
	return f ? static_cast<int>(f - s.c_str()) : -1;
}

const wchar_t* wolf::FindFirstS(const wchar_t *s, wchar_t whatCh, size_t sChars)         { return _FindCh(s, sChars, whatCh, true, false); }
int            wolf::FindFirstS(const wstring& s, wchar_t whatCh, size_t sChars)         { return _FindCh(s, sChars, whatCh, true, false); }
const wchar_t* wolf::FindFirstS(const wchar_t *s, const wchar_t *whatStr, size_t sChars) { return _FindStr(s, sChars, whatStr, true, false); }
int            wolf::FindFirstS(const wstring& s, const wchar_t *whatStr, size_t sChars) { return _FindStr(s, sChars, whatStr, true, false); }
const wchar_t* wolf::FindFirstI(const wchar_t *s, wchar_t whatCh, size_t sChars)         { return _FindCh(s, sChars, whatCh, false, false); }
int            wolf::FindFirstI(const wstring& s, wchar_t whatCh, size_t sChars)         { return _FindCh(s, sChars, whatCh, false, false); }
const wchar_t* wolf::FindFirstI(const wchar_t *s, const wchar_t *whatStr, size_t sChars) { return _FindStr(s, sChars, whatStr, false, false); }
int            wolf::FindFirstI(const wstring& s, const wchar_t *whatStr, size_t sChars) { return _FindStr(s, sChars, whatStr, false, false); }
const wchar_t* wolf::FindLastS(const wchar_t *s, wchar_t whatCh, size_t sChars)          { return _FindCh(s, sChars, whatCh, true, true); }
int            wolf::FindLastS(const wstring& s, wchar_t whatCh, size_t sChars)          { return _FindCh(s, sChars, whatCh, true, true); }
const wchar_t* wolf::FindLastS(const wchar_t *s, const wchar_t *whatStr, size_t sChars)  { return _FindStr(s, sChars, whatStr, true, true); }
int            wolf::FindLastS(const wstring& s, const wchar_t *whatStr, size_t sChars)  { return _FindStr(s, sChars, whatStr, true, true); }
const wchar_t* wolf::FindLastI(const wchar_t *s, wchar_t whatCh, size_t sChars)          { return _FindCh(s, sChars, whatCh, false, true); }
int            wolf::FindLastI(const wstring& s, wchar_t whatCh, size_t sChars)          { return _FindCh(s, sChars, whatCh, false, true); }
const wchar_t* wolf::FindLastI(const wchar_t *s, const wchar_t *whatStr, size_t sChars)  { return _FindStr(s, sChars, whatStr, false, true); }
int            wolf::FindLastI(const wstring& s, const wchar_t *whatStr, size_t sChars)  { return _FindStr(s, sChars, whatStr, false, true); }

wstring& _Replace(wstring& s, const wchar_t *what, const wchar_t *forWhat, bool isCS)
{
	size_t whatLen = lstrlen(what),
		forWhatLen = lstrlen(forWhat);

	int numOccurrences = 0; // how many "what" exist in "s"
	const wchar_t *pS = s.c_str();
	while (pS = _FindStr(pS, 0, what, isCS, false)) {
		++numOccurrences;
		pS += whatLen; // go beyond
	}

	wstring buf(s.length() + numOccurrences * (forWhatLen - whatLen), L'\0');
	wchar_t *pBuf = &buf[0];
	const wchar_t *pBase = s.c_str(), *pOrig = s.c_str();
	pS = s.c_str();

	while (pS = _FindStr(pS, 0, what, isCS, false)) {
		memcpy(pBuf, pOrig, (pS - pBase) * sizeof(wchar_t)); // copy chars until before replacement
		pBuf += pS - pBase;
		memcpy(pBuf, forWhat, forWhatLen * sizeof(wchar_t));
		pBuf += forWhatLen;
		pS += whatLen;
		pOrig += pS - pBase;
		pBase = pS;
	}
	memcpy(pBuf, pOrig, (s.c_str() + s.length() - pOrig) * sizeof(wchar_t)); // copy rest of original string

	s.swap(buf);
	return s;
}
wstring& wolf::ReplaceS(std::wstring& s, const wchar_t *whatStr, const wchar_t *forWhat) { return _Replace(s, whatStr, forWhat, true); }
wstring& wolf::ReplaceI(std::wstring& s, const wchar_t *whatStr, const wchar_t *forWhat) { return _Replace(s, whatStr, forWhat, false); }


wstring wolf::Sprintf(const wchar_t *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int newLen = _vscwprintf(fmt, args); // calculate length, without terminating null
	wstring ret(newLen, L'\0');
	_vsnwprintf(&ret[0], newLen, fmt, args); // do the job
	va_end(args);
	return ret;
}

wstring& wolf::Trim(wstring& s)
{
	TrimNulls(s);

	size_t len = s.size();
	size_t iFirst = 0, iLast = len - 1; // bounds of trimmed string
	bool onlySpaces = true; // our string has only spaces?

	for (size_t i = 0; i < len; ++i) {
		if (!std::iswspace(s[i])) {
			iFirst = i;
			onlySpaces = false;
			break;
		}
	}
	if (onlySpaces) {
		s.clear();
		return s;
	}

	for (size_t i = len; i-- > 0; ) {
		if (!std::iswspace(s[i])) {
			iLast = i;
			break;
		}
	}

	std::copy(std::next(s.begin(), iFirst), // move the non-space chars back
		std::next(s.begin(), iLast + 1), s.begin());
	s.resize(iLast - iFirst + 1); // trim container size
	return s;
}

wstring wolf::ParseUtf8(const BYTE *data, size_t length)
{
	wstring ret;
	if (data && length) {
		int neededLen = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(data),
			static_cast<int>(length), nullptr, 0);
		ret.resize(neededLen);
		MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(data),
			static_cast<int>(length), &ret[0], neededLen);
		TrimNulls(ret);
	}
	return ret;
}

vector<wstring> wolf::Explode(const wstring& s, const wchar_t *delimiters)
{
	// Count how many pieces we'll have after exploding.
	int num = 0;
	const wchar_t *pBase = s.c_str();
	for (;;) {
		size_t lenSub = wcscspn(pBase, delimiters);
		if (lenSub) ++num;
		if (pBase[lenSub] == L'\0') break;
		pBase += lenSub + 1;
	}
	
	vector<wstring> ret(num); // alloc return buffer

	// Grab each substring after explosion.
	num = 0;
	pBase = s.c_str();
	for (;;) {
		size_t lenSub = wcscspn(pBase, delimiters);
		if (lenSub) ret[num++].insert(0, pBase, lenSub);
		if (pBase[lenSub] == L'\0') break;
		pBase += lenSub + 1;
	}
	return ret;
}

vector<wstring> wolf::ExplodeMultiStr(const wchar_t *multiStr)
{
	// Example multiStr:
	// L"first one\0second one\0third one\0"
	// Assumes a well-formed multiStr, which ends with two nulls.

	// Count number of null-delimited strings; string end with double null.
	int numStrings = 0;
	const wchar_t *pRun = multiStr;
	while (*pRun) {
		++numStrings;
		pRun += lstrlen(pRun) + 1;
	}

	// Alloc return array of strings.
	vector<wstring> ret;
	ret.reserve(numStrings);

	// Copy each string.
	pRun = multiStr;
	for (int i = 0; i < numStrings; ++i) {
		ret.emplace_back(pRun);
		pRun += lstrlen(pRun) + 1;
	}

	return ret;
}