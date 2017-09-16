/**
 * Part of WinLamb - Win32 API Lambda Library - More
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/winlamb-more
 */

#pragma once
#include <array>
#include <memory>
#include "str.h"
#pragma comment(lib, "Version.lib")

namespace wl {

// Wraps operations with versions of executable files.
class exe_version final {
public:
	std::array<UINT, 4> version;

	exe_version()
		: version({0, 0, 0, 0}) { }
	explicit exe_version(UINT major, UINT minor = 0, UINT build = 0, UINT revision = 0)
		: version({major, minor, build, revision}) { }

	bool operator==(const exe_version& other) const { return this->version == other.version; }
	bool operator!=(const exe_version& other) const { return !this->operator==(other); }
	bool operator>=(const exe_version& other) const { return this->operator>(other) || this->operator==(other); }
	bool operator<=(const exe_version& other) const { return this->operator<(other) || this->operator==(other); }
	bool operator<(const exe_version& other)  const { return other > *this; }
	bool operator>(const exe_version& other)  const {
		return this->version[0] < other.version[0] ||
			this->version[1] < other.version[1] ||
			this->version[2] < other.version[2] ||
			this->version[3] < other.version[3];
	}

	std::wstring to_string(BYTE numDigits = 4) const {
		std::wstring ret;
		if (numDigits) {
			ret.append(std::to_wstring(version[0]));
			for (size_t i = 1; i < numDigits && i <= 4; ++i) {
				ret.append(L".")
					.append(std::to_wstring(version[i]));
			}
		}
		return ret;
	}

	bool parse(const std::wstring& text) {
		std::vector<std::wstring> fields = str::explode(text, L".");
		std::array<UINT, 4> tmp = {0, 0, 0, 0};
		for (size_t i = 0; i < fields.size() && i <= 4; ++i) {
			if (!str::is_uint(fields[i])) {
				return false;
			}
			tmp[i] = std::stoi(fields[i]);
		}
		return true;
	}

	bool read(const std::wstring& exeOrDll, std::wstring* pErr = nullptr) {
		DWORD szVer = GetFileVersionInfoSizeW(exeOrDll.c_str(), nullptr);
		if (!szVer) {
			if (pErr) *pErr = L"GetFileVersionInfoSize returned zero.";
			return false;
		}

		std::unique_ptr<wchar_t[]> dataBuf = std::make_unique<wchar_t[]>(szVer);

		if (!GetFileVersionInfoW(exeOrDll.c_str(), 0, szVer, dataBuf.get())) {
			if (pErr) *pErr = L"GetFileVersionInfo failed.";
			return false;
		}

		BYTE* lpBuf = nullptr;
		UINT theSize = 0;

		if (!VerQueryValueW(dataBuf.get(), L"\\", reinterpret_cast<void**>(&lpBuf), &theSize)) {
			if (pErr) *pErr = L"VerQueryValue failed.";
			return false;
		}

		if (!theSize) return false;

		VS_FIXEDFILEINFO* verInfo = reinterpret_cast<VS_FIXEDFILEINFO*>(lpBuf);
		if (verInfo->dwSignature != 0xfeef04bd) {
			if (pErr) *pErr = L"VS_FIXEDFILEINFO different from 0xfeef04bd.";
			return false;
		}

		version = {
			(verInfo->dwFileVersionMS >> 16) & 0xffff,
			(verInfo->dwFileVersionMS >>  0) & 0xffff,
			(verInfo->dwFileVersionLS >> 16) & 0xffff,
			(verInfo->dwFileVersionLS >>  0) & 0xffff
		};

		if (pErr) pErr->clear();
		return true;
	}
};

}//namespace wl