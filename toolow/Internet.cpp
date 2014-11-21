//
// Automation for internet related operations.
// Part of TOOLOW - Thin Object Oriented Layer Over Win32.
// @author Rodrigo Cesar de Freitas Dias
// @see https://github.com/rodrigocfd/toolow
//

#include "Internet.h"
#include "System.h"
#pragma comment(lib, "Winhttp.lib")

String Internet::Session::_FormatErr(const wchar_t *funcName, DWORD code)
{
	const wchar_t *s = nullptr;

	switch(code) {
	case ERROR_NOT_ENOUGH_MEMORY:               s = L"not enough memory"; break;
	case ERROR_WINHTTP_CANNOT_CONNECT:          s = L"cannot connect"; break;
	case ERROR_WINHTTP_CHUNKED_ENCODING_HEADER_SIZE_OVERFLOW: s = L"chunked encoding header size overflow"; break;
	case ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED: s = L"client auth cert needed"; break;
	case ERROR_WINHTTP_CONNECTION_ERROR:        s = L"connection error"; break;
	case ERROR_WINHTTP_HEADER_COUNT_EXCEEDED:   s = L"header count exceeded"; break;
	case ERROR_WINHTTP_HEADER_NOT_FOUND:        s = L"header not found"; break;
	case ERROR_WINHTTP_HEADER_SIZE_OVERFLOW:    s = L"header size overflow"; break;
	case ERROR_WINHTTP_INCORRECT_HANDLE_STATE:  s = L"incorrect handle state"; break;
	case ERROR_WINHTTP_INCORRECT_HANDLE_TYPE:   s = L"incorrect handle type"; break;
	case ERROR_WINHTTP_INTERNAL_ERROR:          s = L"internal error"; break;
	case ERROR_WINHTTP_INVALID_SERVER_RESPONSE: s = L"invalid server response"; break;
	case ERROR_WINHTTP_INVALID_URL:             s = L"invalid URL"; break;
	case ERROR_WINHTTP_LOGIN_FAILURE:           s = L"login failure"; break;
	case ERROR_WINHTTP_NAME_NOT_RESOLVED:       s = L"name not resolved"; break;
	case ERROR_WINHTTP_OPERATION_CANCELLED:     s = L"operation cancelled"; break;
	case ERROR_WINHTTP_REDIRECT_FAILED:         s = L"redirect failed"; break;
	case ERROR_WINHTTP_RESEND_REQUEST:          s = L"resend request"; break;
	case ERROR_WINHTTP_RESPONSE_DRAIN_OVERFLOW: s = L"response drain overflow"; break;
	case ERROR_WINHTTP_SECURE_FAILURE:          s = L"secure failure"; break;
	case ERROR_WINHTTP_SHUTDOWN:                s = L"shutdown"; break;
	case ERROR_WINHTTP_TIMEOUT:                 s = L"timeout"; break;
	case ERROR_WINHTTP_UNRECOGNIZED_SCHEME:     s = L"unrecognized scheme or bad URL"; break;
	default:                                    s = nullptr;
	}

	return String::Fmt(L"%s() failed. Error: %s.", funcName,
		s ? s : String::Fmt(L"(unhandled, %08X)", code).str() );
}

bool Internet::Session::_Core::init(const wchar_t *userAgent, String *pErr)
{
	if(!_hSession) {
		// http://social.msdn.microsoft.com/forums/en-US/vclanguage/thread/45ccd91c-6794-4f9b-8f4f-865c76cc146d
		if(!WinHttpCheckPlatform()) {
			if(pErr) *pErr = L"WinHttpCheckPlatform() failed. This platform is not supported by WinHTTP.";
			return false;
		}

		_hSession = WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if(!_hSession) {
			if(pErr) *pErr = _FormatErr(L"WinHttpOpen", GetLastError());
			return false;
		}
	}

	if(pErr) *pErr = L"";
	return true;
}


void Internet::Download::_Worker::addRequestHeaders(initializer_list<const wchar_t*> requestHeaders)
{
	for(int i = 0, sz = (int)requestHeaders.size(); i < sz; ++i)
		_requestHeaders.append( *(requestHeaders.begin() + i) );
}

void Internet::Download::_Worker::processDownload(const String& url, const String& verb, function<void(Msg, const Status*)> callback)
{
	_callback = MOVE(callback); // keep the user callback

	if(!this->_initHandles(url, verb)) return;
	if(!this->_contactServer()) return;
	if(!this->_parseHeaders()) return;

	// Send first good notification to client. The processing keeps going (asynchronous),
	// note the postMessage() instead of sendMessage().
	_pWnd->postFunction([&]() { _callback(Msg::BEGIN, &_status); });

	bool isToFile = !_status.destPath.isEmpty(); // will directly save downloaded data into file?
	File::Raw fout;
	Array<BYTE> fbuf;

	// Prepare destination buffer.
	if(isToFile) {
		if(!this->_prepareFileOutput(fout)) return;
	} else {
		if(_status.contentLength) _status.buffer.realloc(_status.contentLength);
	}

	// Receive the data from server.
	for(;;) {
		DWORD incomingBytes = 0;
		if(!this->_getIncomingByteCount(incomingBytes)) return;
		if(!incomingBytes) break; // no more bytes to be downloaded

		if(isToFile) {
			fbuf.realloc(incomingBytes);
			if(!this->_receiveBytes(incomingBytes, &fbuf[0])) return;
			if(!fout.write(fbuf, &_status.err)) { // data is appended to file
				this->_closeHandles();
				_status.err.insert(0, String::Fmt(L"Failed to write %d received bytes to file.\n", incomingBytes));
				_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
				return;
			}
		} else {
			if(!_status.contentLength) _status.buffer.realloc(_status.totalDownloaded + incomingBytes);
			if(!this->_receiveBytes(incomingBytes, &_status.buffer[0] + _status.totalDownloaded)) return; // data is appended to buffer
		}

		_status.totalDownloaded += incomingBytes;

		// Notify client of the progress. The downloading keeps going (asynchronous),
		// note the PostMessage() instead of SendMessage().
		_pWnd->postFunction([&]() { _callback(Msg::PROGRESS, &_status); });
	}

	fout.close();
	this->_closeHandles();
	_pWnd->sendFunction([&]() { _callback(Msg::END, &_status); });
}

void Internet::Download::_Worker::_closeHandles()
{
	if(_hRequest) {
		WinHttpCloseHandle(_hRequest);
		_hRequest = nullptr;
	}
	if(_hConnect) {
		WinHttpCloseHandle(_hConnect);
		_hConnect = nullptr;
	}
}

bool Internet::Download::_Worker::_initHandles(const String& url, const String& verb)
{
	// Crack the URL.
	DWORD dwErr = ERROR_SUCCESS;
	Url crackedUrl;
	if(!crackedUrl.crack(url, &dwErr)) {
		_status.err = Session::_FormatErr(L"WinHttpCrackUrl", dwErr);
		_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
		return false;
	}

	// Open the connection handle.
	if(!( _hConnect = WinHttpConnect(_sessionCore->hSession(), crackedUrl.host(), crackedUrl.port(), 0) )) {
		_status.err = Session::_FormatErr(L"WinHttpConnect", GetLastError());
		_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
		return false;
	}

	// Build the request handle.
	String fullPath = crackedUrl.pathAndExtra();
	_hRequest = WinHttpOpenRequest(_hConnect, verb.str(), fullPath.str(), nullptr,
		_referrer.isEmpty() ? WINHTTP_NO_REFERER : _referrer.str(),
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		crackedUrl.isHttps() ? WINHTTP_FLAG_SECURE : 0);
	if(!_hRequest) {
		DWORD dwErr = GetLastError();
		this->_closeHandles();
		_status.err = Session::_FormatErr(L"WinHttpOpenRequest", dwErr);
		_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
		return false;
	}

	return true;
}

bool Internet::Download::_Worker::_contactServer()
{
	// Add the request headers to request handle.
	for(int i = 0; i < _requestHeaders.size(); ++i) {
		if(!WinHttpAddRequestHeaders(_hRequest, _requestHeaders[i].str(), (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD)) {
			DWORD dwErr = GetLastError();
			this->_closeHandles();
			_status.err = Session::_FormatErr(L"WinHttpAddRequestHeaders", dwErr);
			_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
			return false;
		}
	}

	// Send the request to server.
	if(!WinHttpSendRequest(_hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
		DWORD dwErr = GetLastError();
		this->_closeHandles();
		_status.err = Session::_FormatErr(L"WinHttpSendRequest", dwErr);
		_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
		return false;
	}

	// Receive the response from server.
	if(!WinHttpReceiveResponse(_hRequest, nullptr)) {
		DWORD dwErr = GetLastError();
		this->_closeHandles();
		_status.err = Session::_FormatErr(L"WinHttpReceiveResponse", dwErr);
		_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
		return false;
	}

	return true;
}

bool Internet::Download::_Worker::_parseHeaders()
{
	// Retrieve the response header.
	DWORD dwSize = 0;
	WinHttpQueryHeaders(_hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX,
		WINHTTP_NO_OUTPUT_BUFFER, &dwSize, WINHTTP_NO_HEADER_INDEX);

	String rawReh; // raw response headers
	rawReh.reserve(dwSize / sizeof(wchar_t));

	if(!WinHttpQueryHeaders(_hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX,
		rawReh.ptrAt(0), &dwSize, WINHTTP_NO_HEADER_INDEX))
	{
		DWORD dwErr = GetLastError();
		this->_closeHandles();
		_status.err = Session::_FormatErr(L"WinHttpQueryHeaders", dwErr);
		_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
		return false;
	}

	// Parse the raw response headers into a hash.
	_status.responseHeaders.removeAll();
	Array<String> lines = rawReh.explode(L"\r\n");
	String key, val;
	key.reserve(32); val.reserve(32); // temp buffers to save reallocs
	for(int i = 0; i < lines.size(); ++i) {
		if(lines[i].isEmpty()) continue;
		int colonIdx = lines[i].findCS(L':');
		if(colonIdx == -1) { // not a key/value pair, probably response line
			_status.responseHeaders[L""] = lines[i]; // empty key
		} else {
			key = lines[i].substr(0, colonIdx);
			val = lines[i].substr(colonIdx + 1, lines[i].len() - (colonIdx + 1));
			_status.responseHeaders[key.trim()] = val.trim();
		}
	}

	// Retrieve content length, if informed by server.
	if(_status.responseHeaders.exists(L"Content-Length")) {
		const String& strContentLength = _status.responseHeaders[L"Content-Length"];
		if(strContentLength.isInt()) // yes, server informed content length
			_status.contentLength = strContentLength.toInt();
	}

	return true;
}

bool Internet::Download::_Worker::_prepareFileOutput(File::Raw& fout)
{
	// If output file already exists, delete it.
	if(File::Exists(_status.destPath)) {
		if(!File::Delete(_status.destPath, &_status.err)) {
			this->_closeHandles();
			_status.err.insert(0, L"Failed to delete existing file.\n");
			_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
			return false;
		}
	}

	// Open the file for writing.
	if(!fout.open(_status.destPath, File::Access::READWRITE, &_status.err)) {
		this->_closeHandles();
		_status.err.insert(0, L"Failed to open output file for writing.\n");
		_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
		return false;
	}

	// Alloc destination file.
	if(_status.contentLength) {
		if(!fout.setNewSize(_status.contentLength, &_status.err)) {
			fout.close();
			this->_closeHandles();
			_status.err.insert(0, L"Failed to resize output file.\n");
			_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
			return false;
		}
	}

	return true;
}

bool Internet::Download::_Worker::_getIncomingByteCount(DWORD& count)
{
	DWORD dwSize = 0;
	if(!WinHttpQueryDataAvailable(_hRequest, &dwSize)) { // how many bytes are about to come
		DWORD dwErr = GetLastError();
		this->_closeHandles();
		_status.err = Session::_FormatErr(L"WinHttpQueryDataAvailable", dwErr);
		_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
		return false;
	}
	count = dwSize;
	return true;
}

bool Internet::Download::_Worker::_receiveBytes(UINT nBytesToRead, BYTE *pDest)
{
	DWORD dwRead = 0;
	if(!WinHttpReadData(_hRequest, (void*)pDest, nBytesToRead, &dwRead)) {
		DWORD dwErr = GetLastError();
		this->_closeHandles();
		_status.err = Session::_FormatErr(L"WinHttpReadData", dwErr);
		_pWnd->sendFunction([&]() { _callback(Msg::FAIL, &_status); });
		return false;
	}
	return true;
}


void Internet::Download::download(String url, String verb, function<void(Msg, const Status*)> callback)
{
	_Worker *_worker2 = _worker;
	_worker = nullptr; // so destructor won't delete our pointer

	System::Thread([=]() {
		_worker2->processDownload(url, verb, MOVE(callback));
		delete _worker2;
	});
}


bool Internet::Url::crack(const wchar_t *address, DWORD *dwErr)
{
	// This helper class simply breaks an URL address into several parts.

	SecureZeroMemory(&_uc, sizeof(_uc));
	_uc.dwStructSize = sizeof(_uc);

	_uc.lpszScheme    = _scheme; _uc.dwSchemeLength    = ARRAYSIZE(_scheme);
	_uc.lpszHostName  = _host;   _uc.dwHostNameLength  = ARRAYSIZE(_host);
	_uc.lpszUserName  = _user;   _uc.dwUserNameLength  = ARRAYSIZE(_user);
	_uc.lpszPassword  = _pwd;    _uc.dwPasswordLength  = ARRAYSIZE(_pwd);
	_uc.lpszUrlPath   = _path;   _uc.dwUrlPathLength   = ARRAYSIZE(_path);
	_uc.lpszExtraInfo = _extra;  _uc.dwExtraInfoLength = ARRAYSIZE(_extra);

	if(!WinHttpCrackUrl(address, 0, 0, &_uc)) {
		if(dwErr) *dwErr = GetLastError();
		return false;
	}

	if(dwErr) *dwErr = ERROR_SUCCESS;
	return true;
}