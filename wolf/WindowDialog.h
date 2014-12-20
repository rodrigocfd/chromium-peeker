/*!
 * All the dialog windows, who take a DLGPROC as a procedure and are created through a dialog resource.
 * Part of WOLF - Win32 Object Lambda Framework.
 * @author Rodrigo Cesar de Freitas Dias
 * @see https://github.com/rodrigocfd/wolf
 */

#pragma once
#include "Window.h"
#include "Resources.h"

//
//           +------------- WindowCtrl <-------------------------+
//           |                                                   +---- [DialogCtrl]
//           |                                                <--+
//           +-- WindowEvent <-- WindowEventDialog <-- Dialog
// Window <--+                                                <--+                  +-- [DialogApp]
//           |                                                   +-- DialogPopup <--+
//           +------------- WindowPopup <------------------------+                  +-- [DialogModal]
//

namespace wolf {

class Dialog : public WindowEventDialog {
public:
	Dialog() : _dialogId(0) { }
	virtual ~Dialog() = 0;
protected:
	int _dialogId;
	void defineDialog(int dialogId) { _dialogId = dialogId; }
	virtual void _internalEvents() = 0;
	static INT_PTR CALLBACK _DialogProc(HWND hdlg, UINT msg, WPARAM wp, LPARAM lp);
};

	class DialogPopup : public Dialog, public WindowPopup {
	private:
		static Font _hSysFont;
	public:
		DialogPopup() : _iconId(0) { }
		virtual ~DialogPopup();
	protected:
		int _iconId, _accelTableId;
		void defineIcon(int iconId)             { _iconId = iconId; }
		void defineAccelTable(int accelTableId) { _accelTableId = accelTableId; }
		virtual void _internalEvents() override;
	};

		// Inherit to create a main dialog-based window.
		class DialogApp : public DialogPopup {
		public:
			virtual ~DialogApp() = 0;
			int run(HINSTANCE hInst, int cmdShow);
		private:
			virtual void _internalEvents() override;
			DialogPopup::_iconId; DialogPopup::_accelTableId;
			Dialog::_dialogId; Dialog::_DialogProc;
		};

		// Inherit to create a popup modal dialog-based window.
		class DialogModal : public DialogPopup {
		public:
			virtual ~DialogModal() = 0;
			int show(WindowPopup *parent);
			BOOL endDialog(INT_PTR nResult) { return ::EndDialog(hWnd(), nResult); }
		private:
			virtual void _internalEvents() override;
			DialogPopup::_iconId; DialogPopup::_accelTableId;
			Dialog::_dialogId; Dialog::_DialogProc;
		};

	// Inherit to create a dialog-based child control.
	class DialogCtrl : public Dialog, public WindowCtrl {
	public:
		virtual ~DialogCtrl() = 0;
		void create(WindowPopup *parent, POINT position, SIZE size);
	private:
		void _internalEvents() override;
		WindowCtrl::_drawBorders;
		Dialog::_dialogId; Dialog::_DialogProc;
	};

}//namespace wolf