
#include <crtdbg.h>
#include "DlgMain.h"

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE h0, LPWSTR cmdLine, int cmdShow)
{
	int ret = 0;
	{
		DlgMain dlgMain;
		dlgMain.run(hInst, cmdShow);
	}
	_ASSERT(!_CrtDumpMemoryLeaks());
	return ret;
}