
#include "Dlg_Dn_Info.h"
#include "../winlamb/str.h"
#include "../winlamb/sys.h"
#include "../winlamb/xml.h"
using namespace wl;
using std::vector;
using std::wstring;

Dlg_Dn_Info::Dlg_Dn_Info(progress_taskbar& taskBar,
	download::session& session, const vector<wstring>& markers)
	: Dlg_Dn(taskBar), _session(session), _markers(markers), _totDownloaded(0)
{
	on.INITDIALOG([&](params::initdialog p)
	{
		init_controls();
		set_text(L"Downloading...");
		sys::thread([&]() {
			_get_one_file(_markers[0]); // proceed with first file
		});
		return TRUE;
	});
}

bool Dlg_Dn_Info::_get_one_file(const wstring& marker)
{
	wstring lnk = L"http://commondatastorage.googleapis.com/chromium-browser-continuous/?delimiter=/&prefix=";
	lnk.append(marker);

	download dl(_session, lnk);
	dl.set_referrer(L"http://commondatastorage.googleapis.com/chromium-browser-continuous/index.html?path=Win/");
	dl.add_request_header({
		L"Accept-Encoding: gzip,deflate,sdch",
		L"Connection: keep-alive",
		L"DNT: 1",
		L"Host: commondatastorage.googleapis.com"
	});

	wstring err;
	if (!dl.start(&err)) {
		return show_err_and_close(L"Error at download start", err);
	}

	vector<BYTE> xmlbuf;
	xmlbuf.reserve(dl.get_content_length());
	while (dl.has_data(&err)) { // each file is small, we don't need to display progress info
		xmlbuf.insert(xmlbuf.end(),
			dl.get_buffer().begin(), dl.get_buffer().end()); // append
	}

	if (!err.empty()) {
		return show_err_and_close(L"Download error", err);
	}

	return _process_file(xmlbuf);
}

bool Dlg_Dn_Info::_process_file(const vector<BYTE>& buf)
{
	_totDownloaded += static_cast<int>(buf.size());
	ui_thread([&]() {
		_label.set_text( str::format(L"%d/%d markers (%.2f KB)...",
			data.size(), _markers.size(),
			static_cast<float>(_totDownloaded) / 1024) );
		double pct = (static_cast<float>(data.size()) / _markers.size()) * 100;
		_progBar.set_pos(pct);
		_taskBar.set_pos(pct);
	});

	wstring xmlStr, err;
	if (!str::parse_blob(buf, xmlStr, &err)) {
		return show_err_and_close(L"XML parsing error", err);
	}
	xml xmlc = xmlStr;
	data.resize(data.size() + 1); // realloc public return buffer

	vector<xml::node*> cnodes = xmlc.root.children_by_name(L"Contents");
	for (xml::node *cnode : cnodes) {
		if (str::ends_withi(cnode->first_child_by_name(L"Key")->value, L"chrome-win32.zip")) {
			data.back().releaseDate = cnode->first_child_by_name(L"LastModified")->value;
			data.back().packageSize = std::stoi(cnode->first_child_by_name(L"Size")->value);
			break;
		}
	}

	if (data.size() == _markers.size()) {
		ui_thread([&]() {
			_taskBar.clear();
			EndDialog(hwnd(), IDOK); // last file has been processed
		});
	} else {
		_get_one_file( _markers[data.size()].c_str() ); // proceed to next file
	}
	return true;
}