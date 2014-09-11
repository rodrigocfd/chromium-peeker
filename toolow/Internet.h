//
// Automation for internet related operations.
// Part of TOOLOW - Thin Object Oriented Layer Over Win32.
// @author Rodrigo Cesar de Freitas Dias
// @see https://github.com/rodrigocfd/toolow
//

#pragma once
#include "File.h"
#include "Hash.h"
#include "Ptr.h"
#include "Window.h"
#include <Winhttp.h>

namespace Internet
{
	enum class Msg { BEGIN=1, PROGRESS, END, FAIL };
	class Download;

	//__________________________________________________________________________________________________
	// All internet operations must belong to a session.
	//
	class Session final {
	private:
		class _Core final {
		private:
			HINTERNET _hSession;
		public:
			_Core()                    : _hSession(nullptr) { }
			~_Core()                   { close(); }
			HINTERNET hSession() const { return _hSession; }
			void      close()          { if(_hSession) { ::WinHttpCloseHandle(_hSession); _hSession = nullptr; } }
			bool      init(const wchar_t *userAgent=L"TOOLOW/1.0", String *pErr=nullptr);
		};

	private:
		friend Download;
		Ptr<_Core> _sessionCore;
	public:
		Session()                  : _sessionCore(new _Core) { }
		HINTERNET hSession() const { return _sessionCore->hSession(); }
		void close()               { _sessionCore->close(); }
		bool init(const wchar_t *userAgent=L"TOOLOW/1.0", String *pErr=nullptr) { return _sessionCore->init(userAgent, pErr); }
		bool init(String *pErr)                                                 { return _sessionCore->init(L"TOOLOW/1.0", pErr); }
	private:
		static String _FormatErr(const wchar_t *funcName, DWORD code);
	};

	//__________________________________________________________________________________________________
	// Holds info to be delivered on download notifications.
	//
	struct Status final {
		WORD         wid;
		Hash<String> responseHeaders;
		Array<BYTE>  buffer;
		String       destPath, err;
		int          contentLength, totalDownloaded;
		Status() : wid(0), contentLength(0), totalDownloaded(0) { }
		float pct() const { return contentLength ? ((float)totalDownloaded / contentLength) * 100 : 0; }
	};

	//__________________________________________________________________________________________________
	// Handles download operations.
	//
	class Download final {
	private:
		struct _Worker final {
		private:
			function<void(Msg, const Status*)> _callback;
			Ptr<Session::_Core> _sessionCore;
			HINTERNET     _hConnect, _hRequest;
			String        _referrer;
			Array<String> _requestHeaders;
			WindowPopup  *_pWnd;
			Status        _status;
		public:
			explicit _Worker(Session& session, WindowPopup *pWnd)
				: _sessionCore(session._sessionCore), _hConnect(nullptr), _hRequest(nullptr), _pWnd(pWnd) { }
			void setUniqueId(WORD wid)                { _status.wid = wid; }
			void setDestFile(const wchar_t *filePath) { _status.destPath = filePath; }
			void setReferrer(const wchar_t *referrer) { _referrer = referrer; }
			void addRequestHeaders(initializer_list<const wchar_t*> requestHeaders);
			void processDownload(const String& url, const String& verb, function<void(Msg, const Status*)> callback);
		private:
			void _closeHandles();
			bool _initHandles(const String& url, const String& verb);
			bool _contactServer();
			bool _parseHeaders();
			bool _prepareFileOutput(File::Raw& fout);
			bool _getIncomingByteCount(DWORD& count);
			bool _receiveBytes(UINT nBytesToRead, BYTE *pDest);
		};

	private:
		_Worker *_worker;
	public:
		explicit Download(Session& session, WindowPopup *pWnd) : _worker(new _Worker(session, pWnd)) { }
		~Download() { if(_worker) delete _worker; }
		void setUniqueId(WORD wid)                { _worker->setUniqueId(wid); }
		void setDestFile(const wchar_t *filePath) { _worker->setDestFile(filePath); }
		void setDestFile(const String& filePath)  { setDestFile(filePath.str()); }
		void setReferrer(const wchar_t *referrer) { _worker->setReferrer(referrer); }
		void setReferrer(const String& referrer)  { setReferrer(referrer.str()); }
		void addRequestHeaders(initializer_list<const wchar_t*> requestHeaders) { _worker->addRequestHeaders(requestHeaders); }
		void download(String url, function<void(Msg, const Status*)> callback)  { download(url, L"GET", MOVE(callback)); }
		void download(String url, String verb, function<void(Msg, const Status*)> callback);
	};

	//__________________________________________________________________________________________________
	// URL cracking.
	//
	class Url final {
	public:
		bool           crack(const wchar_t *address, DWORD *dwErr=nullptr);
		bool           crack(const String& address, DWORD *dwErr=nullptr) { return crack(address.str(), dwErr); }
		const wchar_t* scheme() const       { return _scheme; }
		const wchar_t* host() const         { return _host; }
		const wchar_t* user() const         { return _user; }
		const wchar_t* pwd() const          { return _pwd; }
		const wchar_t* path() const         { return _path; }
		const wchar_t* extra() const        { return _extra; }
		String         pathAndExtra() const { String ret(_path); ret.append(_extra); return ret; }
		int            port() const         { return _uc.nPort; }
		bool           isHttps() const      { return _uc.nScheme == INTERNET_SCHEME_HTTPS; }
	private:
		URL_COMPONENTS _uc;
		wchar_t        _scheme[16], _host[64], _user[64], _pwd[64], _path[256], _extra[256];
	};
}