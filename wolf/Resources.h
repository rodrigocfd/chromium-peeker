/*!
 * Automation for some Win32 resources.
 * Part of WOLF - Win32 Object Lambda Framework.
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/wolf
 */

#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "Window.h"

namespace wolf {

class Font final {
public:
	struct Info final {
		wchar_t name[LF_FACESIZE];
		int     size;
		bool    bold;
		bool    italic;

		Info() : size(0), bold(false), italic(false) { *name = L'\0'; }
		Info(const Info& other) { operator=(other); }
		Info& operator=(const Info& other) {
			size = other.size;
			bold = other.bold;
			italic = other.italic;
			lstrcpy(name, other.name);
			return *this;
		}
	};
private:
	HFONT _hFont;
public:
	Font()                  : _hFont(nullptr) { }
	Font(HFONT hfont)       : _hFont(hfont)   { }
	Font(const Font& other) : Font() { operator=(other); }
	Font(Font&& other)      : Font() { operator=(std::move(other)); }
	~Font()                 { release(); }

	Font& operator=(HFONT hfont)       { release(); _hFont = hfont; return *this; }
	Font& operator=(const Font& other) { release(); cloneFrom(other); return *this; }
	Font& operator=(Font&& other)      { _hFont = other._hFont; other.release(); return *this; }

	HFONT hFont() const            { return _hFont; }
	void  release()                { if (_hFont) { ::DeleteObject(_hFont); _hFont = nullptr; } }
	Font& create(const wchar_t *name, int size, bool bold=false, bool italic=false);
	Font& create(const Info& info) { return create(info.name, info.size, info.bold, info.italic); }
	Font& cloneFrom(const Font& font);
	Info  getInfo() const;
	Font& apply(HWND hWnd);
	Font& applyOnChildren(HWND hWnd);

	static bool Exists(const wchar_t *name);
	static Info GetDefaultDialogFontInfo();
private:
	static void _LogfontToInfo(const LOGFONT& lf, Info& info);
};


class Icon final {
private:
	HICON _hIcon;
public:
	Icon()  : _hIcon(nullptr) { }
	~Icon() { this->free(); }

	HICON hIcon() const          { return _hIcon; }
	Icon& free()                 { if (_hIcon) ::DestroyIcon(_hIcon); return *this; }
	Icon& operator=(HICON hIcon) { _hIcon = hIcon; return *this; }
	Icon& getFromExplorer(const wchar_t *fileExtension);
	Icon& getFromResource(int iconId, int size, HINSTANCE hInst=nullptr);

	static void IconToLabel(HWND hStatic, int idIconRes, BYTE size);
};


class Xml final {
public:
	class Node final {
	public:
		std::wstring name;
		std::wstring value;
		std::unordered_map<std::wstring, std::wstring> attrs;
		std::vector<Node> children;

		std::vector<Node*> getChildrenByName(const wchar_t *elemName);
		Node* firstChildByName(const wchar_t *elemName);
	};
public:
	Node root;

	Xml()                        { }
	Xml(const Xml& other)        : root(other.root) { }
	Xml(Xml&& other)             : root(std::move(other.root)) { }
	Xml(const wchar_t *str)      { parse(str); }
	Xml(const std::wstring& str) { parse(str); }

	Xml& operator=(const Xml& other)    { root = other.root; return *this; }
	Xml& operator=(Xml&& other)         { root = std::move(other.root); return *this; }
	bool parse(const wchar_t *str);
	bool parse(const std::wstring& str) { return parse(str.c_str()); }
	bool load(const wchar_t *file, std::wstring *pErr=nullptr);
};

}//namespace wolf