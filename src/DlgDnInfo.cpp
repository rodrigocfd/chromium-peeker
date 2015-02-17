
#include "DlgDnInfo.h"
#include "../res/resource.h"
using namespace owl;
using std::vector;
using std::wstring;

void DlgDnInfo::onInitDialog()
{
	DlgDn::initCtrls();
	this->setText(L"Downloading...");
	System::Thread([&]() {
		this->doGetOneFile(this->markers[0]); // proceed with first file
	});
}

bool DlgDnInfo::doGetOneFile(const wstring& marker)
{
	wstring lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=";
	lnk.append(marker);

	Internet::Download dl(this->session, lnk);
	dl.setReferrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dl.addRequestHeaders({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err;
	if (!dl.start(&err)) {
		return DlgDn::doShowErrAndClose(L"Error at download start", err);
	}

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.getContentLength());
	while (dl.hasData(&err)) { // each file is small, we don't need to display progress info
		xmlbuf.insert(xmlbuf.end(), dl.getBuffer().begin(), dl.getBuffer().end());
	}

	if (!err.empty()) {
		return DlgDn::doShowErrAndClose(L"Download error", err);
	}

	return this->doProcessFile(xmlbuf);
}

bool DlgDnInfo::doProcessFile(const vector<BYTE>& buf)
{
	this->totDownloaded += static_cast<int>(buf.size());
	this->sendFunction([&]() {
		this->label.setText( Sprintf(L"%d/%d markers (%.2f KB)...",
			this->data.size(), this->markers.size(), static_cast<float>(this->totDownloaded) / 1024) );
		this->progBar.setPos( (static_cast<float>(this->data.size()) / this->markers.size()) * 100 );
	});

	Xml xml = ParseUtf8(buf);
	this->data.resize( this->data.size() + 1 ); // realloc public return buffer

	vector<Xml::Node*> cnodes = xml.root.getChildrenByName(L"Contents");
	for (Xml::Node *cnode : cnodes) {
		if (EndsWith(cnode->firstChildByName(L"Key")->value, L"chrome-win32.zip")) {
			this->data.back().releaseDate = cnode->firstChildByName(L"LastModified")->value;
			this->data.back().packageSize = std::stoi(cnode->firstChildByName(L"Size")->value);
			break;
		}
	}

	if (this->data.size() == this->markers.size()) {
		this->sendFunction([&]() {
			this->endDialog(IDOK); // last file has been processed
		});
	} else {
		this->doGetOneFile( this->markers[this->data.size()].c_str() ); // proceed to next file
	}
	return true;
}