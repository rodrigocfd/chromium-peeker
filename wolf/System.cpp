/*!
 * OS-related functions.
 * Part of WOLF - Win32 Object Lambda Framework.
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/wolf
 */

#include "System.h"
#include "StrUtil.h"
#include <process.h>
#include <Shlobj.h>
#pragma comment(lib, "Shell32.lib") // SHGetFolderPath
using namespace wolf;
using std::function;
using std::wstring;

void System::Thread(function<void()> callback)
{
	// Cheap alternative to std::thread([](){}).detach().

	struct CbPack { function<void()> cb; };
	CbPack *pack = new CbPack{ std::move(callback) };

	HANDLE thandle = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, [](void *ptr)->unsigned int {
		CbPack *pPack = reinterpret_cast<CbPack*>(ptr);
		pPack->cb(); // invoke user callback
		delete pPack;
		_endthreadex(0); // http://www.codeproject.com/Articles/7732/A-class-to-synchronise-thread-completions/
		return 0;
	}, pack, 0, nullptr));

	CloseHandle(thandle);
}

DWORD System::Exec(const wchar_t *cmdLine)
{
	SECURITY_ATTRIBUTES sa = { 0 };
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;

	// Avoid eventual crash under Unicode compiling.
	wchar_t *cmdLine2 = _wcsdup(cmdLine);

	PROCESS_INFORMATION pi = { 0 };
	DWORD dwExitCode = 1; // returned by executed program

	if (CreateProcess(nullptr, cmdLine2, &sa, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
		WaitForSingleObject(pi.hProcess, INFINITE); // the program flow is stopped here to wait
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

	free(cmdLine2);
	return dwExitCode;
}

wstring System::GetExePath()
{
	wchar_t buf[MAX_PATH];
	GetModuleFileName(nullptr, buf, ARRAYSIZE(buf)); // retrieves EXE itself directory

	wstring ret = buf;
	ret.resize(FindLastS(ret, L'\\')); // truncate removing EXE filename and trailing backslash
#ifdef _DEBUG
	ret.resize(FindLastS(ret, L'\\')); // bypass "Debug" folder, remove trailing backslash too
#endif
	return ret;
}

wstring System::GetDesktopPath()
{
	wchar_t buf[MAX_PATH];
	SHGetFolderPath(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, 0, buf); // won't have trailing backslash
	return buf;
}

wstring System::GetMyDocsPath()
{
	wchar_t buf[MAX_PATH];
	SHGetFolderPath(nullptr, CSIDL_MYDOCUMENTS, nullptr, 0, buf); // won't have trailing backslash
	return buf;
}

wstring System::GetRoamingPath()
{
	wchar_t buf[MAX_PATH];
	SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, 0, buf); // won't have trailing backslash
	return buf;
}


System::Date& System::Date::setNow()
{
	SYSTEMTIME st1 = { 0 };
	GetSystemTime(&st1);

	TIME_ZONE_INFORMATION tzi = { 0 };
	GetTimeZoneInformation(&tzi);
	SystemTimeToTzSpecificLocalTime(&tzi, &st1, &_st);

	return *this;
}

System::Date& System::Date::setFromFt(const FILETIME& ft)
{
	SYSTEMTIME st1 = { 0 };
	FileTimeToSystemTime(&ft, &st1);

	TIME_ZONE_INFORMATION tzi = { 0 };
	GetTimeZoneInformation(&tzi);
	SystemTimeToTzSpecificLocalTime(&tzi, &st1, &_st);

	return *this;
}

System::Date& System::Date::setFromMs(LONGLONG ms)
{
	SecureZeroMemory(&_st, sizeof(SYSTEMTIME));
	
	_st.wMilliseconds = ms % 1000;
	ms = (ms - _st.wMilliseconds) / 1000; // now in seconds
	_st.wSecond = ms % 60;
	ms = (ms - _st.wSecond) / 60; // now in minutes
	_st.wMinute = ms % 60;
	ms = (ms - _st.wMinute) / 60; // now in hours
	_st.wHour = ms % 24;
	ms = (ms - _st.wHour) / 24; // now in days

	return *this;
}

LONGLONG System::Date::getTimestamp() const
{
	// http://www.frenk.com/2009/12/convert-filetime-to-unix-timestamp/
	LARGE_INTEGER date, adjust;
	_StToLi(_st, date);
	adjust.QuadPart = 11644473600000 * 10000; // 100-nanoseconds = milliseconds * 10000
	date.QuadPart -= adjust.QuadPart; // removes the diff between 1970 and 1601
	//return date.QuadPart / 10000000; // converts back from 100-nanoseconds to seconds
	return date.QuadPart / 10000; // to milliseconds; to printf use %I64u
}

LONGLONG System::Date::minus(const System::Date &other) const
{
	LARGE_INTEGER liUs, liThem;
	_StToLi(_st, liUs);
	_StToLi(other._st, liThem);
	return (liUs.QuadPart - liThem.QuadPart) / 10000; // 100-nanoseconds to milliseconds; to printf use %I64u
}

System::Date& System::Date::addMs(LONGLONG ms)
{
	LARGE_INTEGER li;
	_StToLi(_st, li);
	li.QuadPart += ms * 10000; // milliseconds to 100-nanoseconds
	_LiToSt(li, _st);
	return *this;
}

void System::Date::_StToLi(const SYSTEMTIME& st, LARGE_INTEGER& li)
{
	FILETIME ft = { 0 };
	SystemTimeToFileTime(&st, &ft);

	li.HighPart = ft.dwHighDateTime;
	li.LowPart = ft.dwLowDateTime;
}

void System::Date::_LiToSt(const LARGE_INTEGER& li, SYSTEMTIME& st)
{
	FILETIME ft = { 0 };
	ft.dwHighDateTime = li.HighPart;
	ft.dwLowDateTime = li.LowPart;

	FileTimeToSystemTime(&ft, &st);
}