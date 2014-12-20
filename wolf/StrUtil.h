/*!
 * String utilities.
 * Part of WOLF - Win32 Object Lambda Framework.
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/wolf
 */

#pragma once
#include <string>
#include <vector>
#include <Windows.h>

namespace wolf {

std::wstring& ToUpper(std::wstring& s);
std::wstring& ToLower(std::wstring& s);
bool          IsInt(const std::wstring& s);
bool          IsFloat(const std::wstring& s);
int         CompareS(const wchar_t *a, const wchar_t *b, size_t nChars=0);
inline int  CompareS(const std::wstring& a, const wchar_t *b, size_t nChars=0)      { return CompareS(a.c_str(), b, nChars); }
inline int  CompareS(const std::wstring& a, const std::wstring& b, size_t nChars=0) { return CompareS(a.c_str(), b.c_str(), nChars); }
int         CompareI(const wchar_t *a, const wchar_t *b, size_t nChars=0);
inline int  CompareI(const std::wstring& a, const wchar_t *b, size_t nChars=0)      { return CompareI(a.c_str(), b, nChars); }
inline int  CompareI(const std::wstring& a, const std::wstring& b, size_t nChars=0) { return CompareI(a.c_str(), b.c_str(), nChars); }
inline bool EqualsS(const std::wstring& a, const wchar_t *b)      { return !CompareS(a, b); }
inline bool EqualsS(const std::wstring& a, const std::wstring& b) { return !CompareS(a, b); }
inline bool EqualsI(const std::wstring& a, const wchar_t *b)      { return !CompareI(a, b); }
inline bool EqualsI(const std::wstring& a, const std::wstring& b) { return !CompareS(a, b); }
bool BeginsWithS(const std::wstring& s, const wchar_t *what);
bool BeginsWithI(const std::wstring& s, const wchar_t *what);
bool EndsWithS(const std::wstring& s, const wchar_t *what);
bool EndsWithI(const std::wstring& s, const wchar_t *what);
const wchar_t* FindFirstS(const wchar_t *s, wchar_t whatCh, size_t sChars=0);
int            FindFirstS(const std::wstring& s, wchar_t whatCh, size_t sChars=0);
const wchar_t* FindFirstS(const wchar_t *s, const wchar_t *whatStr, size_t sChars=0);
int            FindFirstS(const std::wstring& s, const wchar_t *whatStr, size_t sChars=0);
inline int     FindFirstS(const std::wstring& s, const std::wstring& whatStr, size_t sChars=0) { return FindFirstS(s, whatStr.c_str(), sChars); }
const wchar_t* FindFirstI(const wchar_t *s, wchar_t whatCh, size_t sChars=0);
int            FindFirstI(const std::wstring& s, wchar_t whatCh, size_t sChars=0);
const wchar_t* FindFirstI(const wchar_t *s, const wchar_t *whatStr, size_t sChars=0);
int            FindFirstI(const std::wstring& s, const wchar_t *whatStr, size_t sChars=0);
inline int     FindFirstI(const std::wstring& s, const std::wstring& whatStr, size_t sChars=0) { return FindFirstI(s, whatStr.c_str(), sChars); }
const wchar_t* FindLastS(const wchar_t *s, wchar_t whatCh, size_t sChars=0);
int            FindLastS(const std::wstring& s, wchar_t whatCh, size_t sChars=0);
const wchar_t* FindLastS(const wchar_t *s, const wchar_t *whatStr, size_t sChars=0);
int            FindLastS(const std::wstring& s, const wchar_t *whatStr, size_t sChars=0);
inline int     FindLastS(const std::wstring& s, const std::wstring& whatStr, size_t sChars=0) { return FindLastS(s, whatStr.c_str(), sChars); }
const wchar_t* FindLastI(const wchar_t *s, wchar_t whatCh, size_t sChars=0);
int            FindLastI(const std::wstring& s, wchar_t whatCh, size_t sChars=0);
const wchar_t* FindLastI(const wchar_t *s, const wchar_t *whatStr, size_t sChars=0);
int            FindLastI(const std::wstring& s, const wchar_t *whatStr, size_t sChars=0);
inline int     FindLastI(const std::wstring& s, const std::wstring& whatStr, size_t sChars=0) { return FindLastI(s, whatStr.c_str(), sChars); }
std::wstring&        ReplaceS(std::wstring& s, const wchar_t *what, const wchar_t *forWhat);
inline std::wstring& ReplaceS(std::wstring& s, const std::wstring& what, const wchar_t *forWhat) { return ReplaceS(s, what.c_str(), forWhat); }
std::wstring&        ReplaceI(std::wstring& s, const wchar_t *whatStr, const wchar_t *forWhat);
inline std::wstring& ReplaceI(std::wstring& s, const std::wstring& what, const wchar_t *forWhat) { return ReplaceI(s, what.c_str(), forWhat); }
std::wstring         Sprintf(const wchar_t *fmt, ...);
std::wstring&        Trim(std::wstring& s);
inline std::wstring& TrimNulls(std::wstring& s)               { if (!s.empty()) s.resize(::lstrlen(s.c_str())); return s; }
std::wstring         ParseUtf8(const BYTE *data, size_t length);
inline std::wstring  ParseUtf8(const std::vector<BYTE>& data) { return ParseUtf8(&data[0], data.size()); }
std::vector<std::wstring> Explode(const std::wstring& s, const wchar_t *delimiters);
std::vector<std::wstring> ExplodeMultiStr(const wchar_t *multiStr);

}//namespace wolf