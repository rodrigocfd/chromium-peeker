/*!
 * HWND wrapper and related classes.
 * Part of WOLF - Win32 Object Lambda Framework.
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/wolf
 */

#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>
#include <CommCtrl.h>

namespace wolf {

// Base HWND wrapper.
class Window {
private:
	HWND _hWnd;
public:
	Window()                    : _hWnd(0) { }
	Window(HWND hwnd)           : _hWnd(hwnd) { }
	Window(const Window& other) : _hWnd(other._hWnd) { }

	HWND          hWnd() const                                      { return _hWnd; }
	Window&       operator=(HWND hwnd)                              { _hWnd = hwnd; return *this; }
	Window&       operator=(const Window& other)                    { _hWnd = other._hWnd; return *this; }
	Window        getParent() const                                 { return ::GetParent(_hWnd); }
	HINSTANCE     getInstance() const                               { return reinterpret_cast<HINSTANCE>(::GetWindowLongPtr(_hWnd, GWLP_HINSTANCE)); }
	int           getId() const                                     { return ::GetDlgCtrlID(_hWnd); }
	LRESULT       sendMessage(UINT msg, WPARAM wp, LPARAM lp) const { return ::SendMessage(_hWnd, msg, wp, lp); }
	LRESULT       postMessage(UINT msg, WPARAM wp, LPARAM lp) const { return ::PostMessage(_hWnd, msg, wp, lp); }
	Window&       setText(const wchar_t *text)                      { ::SetWindowText(_hWnd, text); return *this; }
	Window&       setText(const std::wstring& text)                 { return setText(text.c_str()); }
	wchar_t*      getText(wchar_t *pBuf, int szBuf) const           { ::GetWindowText(_hWnd, pBuf, szBuf); return pBuf; }
	std::wstring& getText(std::wstring& buf) const;
	std::wstring  getText() const                                   { std::wstring ret; getText(ret); return ret; }
	Window&       setPos(HWND hInsertAfter, int x, int y, int cx, int cy, UINT flags) { ::SetWindowPos(_hWnd, hInsertAfter, x, y, cx, cy, flags); return *this; }
	Window&       setFocus()                                        { ::SetFocus(_hWnd); return *this; }
	bool          isFocused() const                                 { return ::GetFocus() == _hWnd; }
	Window&       setVisible(bool visible)                          { ::ShowWindow(_hWnd, visible ? SW_SHOW : SW_HIDE); return *this; }
	bool          isVisible() const                                 { return ::IsWindowVisible(_hWnd) == TRUE; }
	Window&       setEnable(bool doEnable)                          { ::EnableWindow(_hWnd, doEnable); return *this; }
	bool          isEnabled() const                                 { return ::IsWindowEnabled(_hWnd) == TRUE; }
	RECT          getClientRect() const                             { RECT r = { 0 }; ::GetClientRect(_hWnd, &r); return r; }
	RECT          getWindowRect() const                             { RECT r = { 0 }; ::GetWindowRect(_hWnd, &r); return r; }
	Window&       screenToClient(POINT *pPt)                        { ::ScreenToClient(_hWnd, pPt); return *this; }
	Window&       screenToClient(RECT *pRc)                         { screenToClient(reinterpret_cast<POINT*>(&pRc->left)); return screenToClient(reinterpret_cast<POINT*>(&pRc->right)); }
	Window&       clientToScreen(POINT *pPt)                        { ::ClientToScreen(_hWnd, pPt); return *this; }
	Window&       clientToScreen(RECT *pRc)                         { clientToScreen(reinterpret_cast<POINT*>(&pRc->left)); return clientToScreen(reinterpret_cast<POINT*>(&pRc->right)); }
};

	// Mix-in class to any popup window.
	class WindowPopup : virtual public Window {
	public:
		virtual ~WindowPopup() = 0;
		void sendFunction(std::function<void()> callback) { _sendOrPostFunction(std::move(callback), true); }
		void postFunction(std::function<void()> callback) { _sendOrPostFunction(std::move(callback), false); }
	protected:
		Window getChild(int id) { return Window(::GetDlgItem(hWnd(), id)); }
		bool   isMinimized()    { return ::IsIconic(hWnd()) == TRUE; }
		bool   isMaximized()    { return ::IsZoomed(hWnd()) == TRUE; }
		int    messageBox(const wchar_t *caption, const wchar_t *body, UINT uType=0);
		int    messageBox(const wchar_t *caption, const std::wstring& body, UINT uType=0)      { return messageBox(caption, body.c_str(), uType); }
		int    messageBox(const std::wstring& caption, const std::wstring& body, UINT uType=0) { return messageBox(caption.c_str(), body.c_str(), uType); }
		bool   getFileOpen(const wchar_t *filter, std::wstring& buf);
		bool   getFileOpen(const wchar_t *filter, std::vector<std::wstring>& arrBuf);
		bool   getFileSave(const wchar_t *filter, std::wstring& buf, const wchar_t *defFile=nullptr);
		bool   getFolderChoose(std::wstring& buf);
		void   setXButton(bool enable);
		void   _setWheelHoverBehavior();
		void   _handleSendOrPostFunction(LPARAM lp);
		enum { SENDORPOSTMSG = WM_APP-1 };
	private:
		void   _sendOrPostFunction(std::function<void()> callback, bool isSend);
	};

	// Mix-in class to any child window.
	class WindowCtrl : virtual public Window {
	public:
		virtual ~WindowCtrl() = 0;
		virtual void create(WindowPopup *parent, POINT position, SIZE size) = 0;
	protected:
		bool _drawBorders(WPARAM wp, LPARAM lp);
	};


// Base class for event processing.
template<typename RET, RET RETVAL, UINT CREATEMSG> class WindowEvent : virtual public Window {
private:
	std::unordered_map<UINT, std::function<RET(WPARAM wp, LPARAM lp)>>             _msgs;
	std::vector<std::function<void()>>                                             _msgsCreateOrInitDialog;
	std::unordered_map<WORD, std::function<void()>>                                _msgsCommand;
	std::unordered_map<int, std::unordered_map<UINT, std::function<void(NMHDR*)>>> _msgsNotify;
public:
	virtual ~WindowEvent() = 0;
	void onMsg(UINT msg, std::function<RET(WPARAM wp, LPARAM lp)> callback) {
		auto em = _msgs.emplace(msg, std::move(callback));
		if (!em.second) em.first->second = std::move(callback); // msg entry already exists, replace the lambda
	}
	void onCreate(std::function<void()> callback)     { _msgsCreateOrInitDialog.emplace_back(std::move(callback)); }
	void onInitDialog(std::function<void()> callback) { _msgsCreateOrInitDialog.emplace_back(std::move(callback)); }
	void onCommand(WORD cmd, std::function<void()> callback) {
		auto em = _msgsCommand.emplace(cmd, std::move(callback));
		if (!em.second) em.first->second = std::move(callback); // cmd entry already exists, replace the lambda
	}
	void onCommand(std::initializer_list<WORD> cmds, std::function<void()> callback) { for (WORD cmd : cmds) onCommand(cmd, callback); }
	void onNotify(int idFrom, UINT code, std::function<void(NMHDR*)> callback) {
		auto emId = _msgsNotify.emplace(idFrom, std::unordered_map<UINT, std::function<void(NMHDR*)>>()); // new idFrom entry
		auto emCode = emId.first->second.emplace(code, std::move(callback)); // new code entry
		if (!emCode.second) emCode.first->second = std::move(callback); // code entry already exists, replace the lambda
	}
	void onNotify(int idFrom, std::initializer_list<UINT> codes, std::function<void(NMHDR*)> callback) { for (UINT code : codes) onNotify(idFrom, code, callback); }
	void onDropFiles(std::function<void(std::vector<std::wstring>& files)> callback) {
		this->onMsg(WM_DROPFILES, [=](WPARAM wp, LPARAM lp)->RET {
			HDROP hDrop = reinterpret_cast<HDROP>(wp);
			std::vector<std::wstring> files(::DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0));
			for (size_t i = 0; i < files.size(); ++i) {
				files[i].resize(::DragQueryFile(hDrop, static_cast<UINT>(i), nullptr, 0) + 1, L'\0'); // alloc path string
				::DragQueryFile(hDrop, static_cast<UINT>(i), &files[i][0], static_cast<UINT>(files[i].size()));
				TrimNulls(files[i]);
			}
			::DragFinish(hDrop);
			callback(files);
			return RETVAL;
		});
	}
	void onInitMenuPopup(std::function<void(HMENU hMenu)> callback) {
		this->onMsg(WM_INITMENUPOPUP, [=](WPARAM wp, LPARAM lp)->RET {
			callback(reinterpret_cast<HMENU>(wp));
			return RETVAL;
		});
	}
protected:
	virtual void events() = 0;
	struct _MsgRes { bool processed; RET res; };

	_MsgRes _processMessage(UINT msg, WPARAM wp, LPARAM lp) {
		switch (msg) {
		case WM_COMMAND: {
			auto itCmd = _msgsCommand.find(LOWORD(wp)); // match command ID
			if (itCmd != _msgsCommand.end()) itCmd->second();
			return { true, RETVAL }; // no further processing
		} case WM_NOTIFY: {
			NMHDR *nmhdr = reinterpret_cast<NMHDR*>(lp);
			auto itFrom = _msgsNotify.find(static_cast<int>(nmhdr->idFrom)); // match idFrom
			if (itFrom != _msgsNotify.end()) {
				auto itCode = itFrom->second.find(nmhdr->code); // match code
				if (itCode != itFrom->second.end()) itCode->second(nmhdr);
			}
			return { true, RETVAL }; // no further processing
		} case CREATEMSG: { // WM_CREATE or WM_INITDIALOG
			for (auto it = _msgsCreateOrInitDialog.rbegin(); it != _msgsCreateOrInitDialog.rend(); ++it)
				(*it)(); // will execute user's first, then internal ones
			return { true, RETVAL }; // no further processing
		} default : {
			auto itMsg = _msgs.find(msg); // match the message within the stored ones
			if (itMsg != _msgs.end())
				return { true, itMsg->second(wp, lp) }; // if user processed the message, no further processing
		} }
		return { false, -1 }; // message not processed here
	}
};
template<typename RET, RET RETVAL, UINT CREATEMSG> WindowEvent<RET, RETVAL, CREATEMSG>::~WindowEvent() { }

	using WindowEventFrame = WindowEvent<LRESULT, 0, WM_CREATE>;
	using WindowEventDialog = WindowEvent<INT_PTR, TRUE, WM_INITDIALOG>;

}//namespace wolf