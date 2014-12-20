/*!
 * Automation for some Win32 resources.
 * Part of WOLF - Win32 Object Lambda Framework.
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/wolf
 */

#pragma warning(disable:4996) // GetVersionEx is deprecated for Win8.1, won't affect current behaviour
#include "Resources.h"
#include "StrUtil.h"
#include "File.h"
#include <MsXml2.h>
#pragma comment(lib, "msxml2.lib")
using namespace wolf;
using std::unordered_map;
using std::vector;
using std::wstring;

Font& Font::create(const wchar_t *name, int size, bool bold, bool italic)
{
	this->release();

	LOGFONT lf = { 0 };
	lstrcpy(lf.lfFaceName, name);
	lf.lfHeight = -(size + 3);
	lf.lfWeight = bold ? FW_BOLD : FW_DONTCARE;
	lf.lfItalic = static_cast<BYTE>(italic);
	_hFont = CreateFontIndirect(&lf);
	return *this;
}

Font& Font::cloneFrom(const Font& font)
{
	this->release();

	LOGFONT lf = { 0 };
	GetObject(font._hFont, sizeof(LOGFONT), &lf);
	_hFont = CreateFontIndirect(&lf);
	return *this;
}

Font::Info Font::getInfo() const
{
	LOGFONT lf = { 0 };
	GetObject(_hFont, sizeof(LOGFONT), &lf);

	Info info;
	_LogfontToInfo(lf, info);
	return info;
}

Font& Font::apply(HWND hWnd)
{
	if (_hFont)
		SendMessage(hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(_hFont), MAKELPARAM(FALSE, 0)); // to window itself only
	return *this;
}

Font& Font::applyOnChildren(HWND hWnd)
{
	if (_hFont) {
		// http://stackoverflow.com/questions/18367641/use-createthread-with-a-lambda
		EnumChildWindows(hWnd, [](HWND hWnd, LPARAM lp)->BOOL { // propagate to children
			SendMessage(hWnd, WM_SETFONT,
				reinterpret_cast<WPARAM>(reinterpret_cast<HFONT>(lp)),
				MAKELPARAM(FALSE, 0)); // will run on each child
			return TRUE;
		}, reinterpret_cast<LPARAM>(_hFont));
	}
	return *this;
}

bool Font::Exists(const wchar_t *name)
{
	// http://cboard.cprogramming.com/windows-programming/90066-how-determine-if-font-support-unicode.html
	bool isInstalled = false;
	HDC hdc = GetDC(nullptr);
	EnumFontFamilies(hdc, name,
		(FONTENUMPROC)[](const LOGFONT *lpelf, const TEXTMETRIC *lpntm, DWORD fontType, LPARAM lp)->int {
			bool *pIsInstalled = reinterpret_cast<bool*>(lp);
			*pIsInstalled = true; // if we're here, font does exist
			return 0;
		}, reinterpret_cast<LPARAM>(&isInstalled));
	ReleaseDC(nullptr, hdc);
	return isInstalled;
}

Font::Info Font::GetDefaultDialogFontInfo()
{
	OSVERSIONINFO ovi = { 0 };
	ovi.dwOSVersionInfoSize = sizeof(ovi);
	GetVersionEx(&ovi);

	NONCLIENTMETRICS ncm = { 0 };
	ncm.cbSize = sizeof(ncm);
	if (ovi.dwMajorVersion < 6) // below Vista
		ncm.cbSize -= sizeof(ncm.iBorderWidth);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0); // default system font

	Info info;
	_LogfontToInfo(ncm.lfMenuFont, info);
	return info;
}

void Font::_LogfontToInfo(const LOGFONT& lf, Font::Info& info)
{
	lstrcpy(info.name, lf.lfFaceName);
	info.size = -(lf.lfHeight + 3);
	info.bold = (lf.lfWeight == FW_BOLD);
	info.italic = (lf.lfItalic == TRUE);
}


Icon& Icon::getFromExplorer(const wchar_t *fileExtension)
{
	this->free();
	wchar_t extens[10];
	lstrcpy(extens, L"*.");
	lstrcat(extens, fileExtension); // prefix extension; caller must pass just the 3 letters (or 4, whathever)
	SHFILEINFO shfi = { 0 };
	SHGetFileInfo(extens, FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi),
		SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
	SHGetFileInfo(extens, FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi),
		SHGFI_ICON | SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
	_hIcon = shfi.hIcon;
	return *this;
}

Icon& Icon::getFromResource(int iconId, int size, HINSTANCE hInst)
{
	// The size should be 16, 32, 48 or any other size the icon eventually has.
	_hIcon = static_cast<HICON>(LoadImage(hInst ? hInst : GetModuleHandle(nullptr),
		MAKEINTRESOURCE(iconId), IMAGE_ICON, size, size, LR_DEFAULTCOLOR) );
	return *this;
}

void Icon::IconToLabel(HWND hStatic, int idIconRes, BYTE size)
{
	// Loads an icon resource into a static control placed on a dialog.
	// On the resource editor, change "Type" property to "Icon".
	// The size should be 16, 32, 48 or any other size the icon eventually has.
	SendMessage(hStatic, STM_SETIMAGE, IMAGE_ICON,
		reinterpret_cast<LPARAM>(static_cast<HICON>( LoadImage(
			reinterpret_cast<HINSTANCE>(GetWindowLongPtr(GetParent(hStatic), GWLP_HINSTANCE)),
			MAKEINTRESOURCE(idIconRes), IMAGE_ICON, size, size, LR_DEFAULTCOLOR) )));
}


static void _ReadAttrs(IXMLDOMNode *xmlnode, unordered_map<wstring, wstring>& attrbuf)
{
	// Read attribute collection.
	IXMLDOMNamedNodeMap *attrs = nullptr;
	xmlnode->get_attributes(&attrs);
	
	long attrCount = 0;
	attrs->get_length(&attrCount);
	attrbuf.clear();
	attrbuf.reserve(attrCount);

	for (long i = 0; i < attrCount; ++i) {
		IXMLDOMNode *attr = nullptr;
		attrs->get_item(i, &attr);

		DOMNodeType type = NODE_INVALID;
		attr->get_nodeType(&type);
		if (type == NODE_ATTRIBUTE) {
			BSTR bstr = nullptr;
			attr->get_nodeName(&bstr); // get attribute name

			VARIANT var = { 0 };
			attr->get_nodeValue(&var); // get attribute value
			
			attrbuf.emplace(static_cast<wchar_t*>(bstr), static_cast<wchar_t*>(var.bstrVal)); // add hash entry
			SysFreeString(bstr);
			VariantClear(&var);
		}
		attr->Release();
	}
	attrs->Release();
}

static int _CountChildNodes(IXMLDOMNodeList *nodeList)
{
	int childCount = 0;
	long totalCount = 0;
	nodeList->get_length(&totalCount); // includes text and actual element nodes
	
	for (long i = 0; i < totalCount; ++i) {
		IXMLDOMNode *child = nullptr;
		nodeList->get_item(i, &child);

		DOMNodeType type = NODE_INVALID;
		child->get_nodeType(&type);
		if (type == NODE_ELEMENT)
			++childCount;

		child->Release();
	}
	return childCount;
}

static void _BuildNode(IXMLDOMNode *xmlnode, Xml::Node& nodebuf)
{
	// Get node name.
	BSTR bstr = nullptr;
	xmlnode->get_nodeName(&bstr);
	nodebuf.name = static_cast<wchar_t*>(bstr);
	SysFreeString(bstr);

	// Parse attributes of node, if any.
	_ReadAttrs(xmlnode, nodebuf.attrs);

	// Process children, if any.
	VARIANT_BOOL vb = FALSE;
	xmlnode->hasChildNodes(&vb);
	if (vb) {
		IXMLDOMNodeList *nodeList = nullptr;
		xmlnode->get_childNodes(&nodeList);
		nodebuf.children.resize(_CountChildNodes(nodeList));

		int childCount = 0;
		long totalCount = 0;
		nodeList->get_length(&totalCount);

		for (long i = 0; i < totalCount; ++i) {
			IXMLDOMNode *child = nullptr;
			nodeList->get_item(i, &child);

			// Node can be text or an actual child node.
			DOMNodeType type = NODE_INVALID;
			child->get_nodeType(&type);
			if (type == NODE_TEXT) {
				xmlnode->get_text(&bstr);
				nodebuf.value.append(static_cast<wchar_t*>(bstr));
				SysFreeString(bstr);
			} else if (type == NODE_ELEMENT) {
				_BuildNode(child, nodebuf.children[childCount++]); // recursively
			} else {
				// (L"Unhandled node type: %d.\n", type);
			}
			child->Release();
		}
		nodeList->Release();
	} else {
		// Assumes that only a leaf node can have text.
		xmlnode->get_text(&bstr);
		nodebuf.value = static_cast<wchar_t*>(bstr);
		SysFreeString(bstr);
	}
}

vector<Xml::Node*> Xml::Node::getChildrenByName(const wchar_t *elemName)
{
	int howMany = 0;
	size_t firstIndex = -1, lastIndex = -1;
	for (size_t i = 0; i < this->children.size(); ++i) {
		if (EqualsI(this->children[i].name, elemName)) { // case-insensitive match
			++howMany;
			if (firstIndex == -1) firstIndex = i;
			lastIndex = i;
		}
	}

	vector<Node*> nodeBuf;
	nodeBuf.reserve(howMany); // alloc return array

	howMany = 0;
	for (size_t i = firstIndex; i <= lastIndex; ++i)
		if (EqualsI(this->children[i].name, elemName))
			nodeBuf.emplace_back(&this->children[i]);
	
	return nodeBuf;
}

Xml::Node* Xml::Node::firstChildByName(const wchar_t *elemName)
{
	for (Node& node : this->children)
		if (EqualsI(node.name, elemName)) // case-insensitive match
			return &node;

	return nullptr; // not found
}

bool Xml::parse(const wchar_t *str)
{
	CoInitialize(nullptr); // http://stackoverflow.com/questions/7824383/double-calls-to-coinitialize
	
	// Create COM object for XML document.
	IXMLDOMDocument2 *doc = nullptr;
	CoCreateInstance(CLSID_DOMDocument30, nullptr, CLSCTX_INPROC_SERVER,
		IID_IXMLDOMDocument, reinterpret_cast<void**>(&doc));
	doc->put_async(FALSE);

	// Parse the XML string.
	VARIANT_BOOL vb = FALSE;
	doc->loadXML(static_cast<BSTR>(const_cast<wchar_t*>(str)), &vb);

	// Get document element and root node from XML.
	IXMLDOMElement *docElem = nullptr;
	doc->get_documentElement(&docElem);

	IXMLDOMNode *rootNode = nullptr;
	docElem->QueryInterface(IID_IXMLDOMNode, reinterpret_cast<void**>(&rootNode));
	_BuildNode(rootNode, this->root); // recursive

	rootNode->Release(); // must be released before CoUninitialize
	docElem->Release();
	doc->Release();
	CoUninitialize();
	return true;
}

bool Xml::load(const wchar_t *file, wstring *pErr)
{
	File::Text fin;
	if (!fin.load(file, pErr)) return false;
	return this->parse(fin.text());
}