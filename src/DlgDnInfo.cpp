
#include "DlgDnInfo.h"
using namespace wolf;
using namespace wolf::file;
using std::vector;
using std::wstring;

void DlgDnInfo::events()
{
	this->onMessage(WM_INITDIALOG, [&](WPARAM wp, LPARAM lp)->INT_PTR
	{
		DlgDn::_initCtrls();
		this->setText(L"Downloading...");
		sys::Thread([&]() {
			this->_doGetOneFile(_markers[0]); // proceed with first file
		});
		return TRUE;
	});
}

bool DlgDnInfo::_doGetOneFile(const wstring& marker)
{
	wstring lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=";
	lnk.append(marker);

	net::Download dl(_session, lnk);
	dl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dl.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err;
	if (!dl.start(&err)) {
		return DlgDn::_doShowErrAndClose(L"Error at download start", err);
	}

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.getContentLength());
	while (dl.hasData(&err)) { // each file is small, we don't need to display progress info
		vec::Append(xmlbuf, dl.getBuffer());
	}

	if (!err.empty()) {
		return DlgDn::_doShowErrAndClose(L"Download error", err);
	}

	return this->_doProcessFile(xmlbuf);
}

bool DlgDnInfo::_doProcessFile(const vector<BYTE>& buf)
{
	_totDownloaded += static_cast<int>(buf.size());
	this->inOrigThread([&]() {
		_label.setText( str::Sprintf(L"%d/%d markers (%.2f KB)...",
			data.size(), _markers.size(), static_cast<float>(_totDownloaded) / 1024) );
		_progBar.setPos( (static_cast<float>(this->data.size()) / _markers.size()) * 100 );
	});

	Xml xml = str::ParseUtf8(buf);
	this->data.resize( this->data.size() + 1 ); // realloc public return buffer

	vector<Xml::Node*> cnodes = xml.root.getChildrenByName(L"Contents");
	for (Xml::Node *cnode : cnodes) {
		if (str::EndsWith(str::Sens::YES, cnode->firstChildByName(L"Key")->value, L"chrome-win32.zip")) {
			this->data.back().releaseDate = cnode->firstChildByName(L"LastModified")->value;
			this->data.back().packageSize = std::stoi(cnode->firstChildByName(L"Size")->value);
			break;
		}
	}

	if (data.size() == _markers.size()) {
		this->inOrigThread([&]() {
			this->endDialog(IDOK); // last file has been processed
		});
	} else {
		this->_doGetOneFile( _markers[this->data.size()].c_str() ); // proceed to next file
	}
	return true;
}