/*!
 * All the regular windows, who take a WNDPROC as a procedure and are created through CreateWindowEx().
 * Part of WOLF - Win32 Object Lambda Framework.
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/wolf
 */

#pragma once
#include "Window.h"
#include "Resources.h"
#include "System.h"

//
//           +------------- WindowCtrl <-----------------------+
//           |                                                 +---- [FrameCtrl]
//           |                                              <--+
//           +-- WindowEvent <-- WindowEventFrame <-- Frame
// Window <--+                                              <--+                 +-- [FrameApp]
//           |                                                 +-- FramePopup <--+
//           +------------- WindowPopup <----------------------+                 +-- [FrameModal]
//

namespace wolf {

class Frame : public WindowEventFrame {
public:
	Frame() : _atom(0) { }
	virtual ~Frame() = 0;
	void invalidateRect(bool bgErase=true) { ::InvalidateRect(hWnd(), 0, bgErase); }
	static ATOM Register(const wchar_t *className, int iconId=0, System::Cursor cursor=System::Cursor::ARROW, System::Color bg=System::Color::BUTTON);
protected:
	ATOM _atom;
	void defineAtom(ATOM atom) { _atom = atom; }
	virtual void _internalEvents() = 0;
	static LRESULT CALLBACK _WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
};

	class FramePopup : public Frame, public WindowPopup {
	private:
		static Font _hSysFont;
		HWND _hWndCurFocus;
	public:
		FramePopup() : _size({100,100}), _acceptDrop(false), _resizable(false), _maximizable(false), _hAccel(nullptr) { }
		virtual ~FramePopup() = 0;
	protected:
		bool _acceptDrop, _resizable, _maximizable;
		SIZE _size;
		HACCEL _hAccel;
		std::wstring _title;
		void defineSize(SIZE sz)                    { _size = sz; }
		void defineAcceptDrop(bool acceptDrop)      { _acceptDrop = acceptDrop; }
		void defineResizable(bool resizable)        { _resizable = resizable; }
		void defineMaximizable(bool maximizable)    { _maximizable = maximizable; }
		void defineTitle(const wchar_t *title)      { _title = title; }
		void defineTitle(const std::wstring& title) { _title = title; }
		void defineAccelTable(HACCEL hAccel)        { _hAccel = hAccel; }
		Window createChild(const wchar_t *className, int id, const wchar_t *caption, DWORD exStyle, DWORD style, POINT pos, SIZE size, LPVOID lp=nullptr);
		virtual void _internalEvents() override;
	};

		// Inherit to create a main frame window, built with CreateWindowEx().
		class FrameApp : public FramePopup {
		public:
			virtual ~FrameApp() = 0;
			int run(HINSTANCE hInst, int cmdShow);
		private:
			void _internalEvents() override;
			FramePopup::_size; FramePopup::_acceptDrop; FramePopup::_resizable; FramePopup::_maximizable; FramePopup::_title;
			Frame::_atom; Frame::_WindowProc;
		};

		// Inherit to create a popup modal frame window, built with CreateWindowEx().
		class FrameModal : public FramePopup {
		public:
			virtual ~FrameModal() = 0;
			int show(WindowPopup *parent);
		private:
			void _internalEvents() override;
			FramePopup::_size; FramePopup::_acceptDrop; FramePopup::_resizable; FramePopup::_maximizable; FramePopup::_title;
			Frame::_atom; Frame::_WindowProc;
		};

	// Inherit to create a frame child control, built with CreateWindowEx().
	class FrameCtrl : public Frame, public WindowCtrl {
	public:
		enum class ScrollBar { NONE=0, HORZ=WS_HSCROLL, VERT=WS_VSCROLL, BOTH=(WS_HSCROLL|WS_VSCROLL) };
	public:
		FrameCtrl() : _id(0), _border(false), _hasChildren(false), _tabStop(true), _scrollBar(FrameCtrl::ScrollBar::NONE) { }
		virtual ~FrameCtrl() = 0;
		void create(WindowPopup *parent, POINT position, SIZE size);
		SCROLLINFO getScrollInfo(int fnBar) { SCROLLINFO si = { 0 }; ::GetScrollInfo(hWnd(), fnBar, &si); return si; }
		int setScrollInfo(int fnBar, const SCROLLINFO& si, bool redraw) { return ::SetScrollInfo(hWnd(), fnBar, &si, (BOOL)redraw); }
	private:
		int _id;
		bool _border, _hasChildren, _tabStop;
		ScrollBar _scrollBar;
	protected:
		void defineId(int id)                    { _id = id; }
		void defineBorder(bool hasBorder)        { _border = hasBorder; }
		void defineHasChildren(bool hasChildren) { _hasChildren = hasChildren; }
		void defineTabStop(bool tabStop)         { _tabStop = tabStop; }
		void defineScrollBar(ScrollBar type)     { _scrollBar = type; }
	private:
		void _internalEvents() override;
		WindowCtrl::_drawBorders;
		Frame::_atom; Frame::_WindowProc;
	};

}//namespace wolf