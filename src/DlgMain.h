
#include "../wolf/wolf.h"
#include "ChromiumRel.h"
using namespace wolf;

class DlgMain final : public DialogApp {
private:
	ChromiumRel       chromiumRel;
	ListView          listview;
	Menu              listMenu;
	Window            lblLoaded;
	Resizer           resz;
	Internet::Session session;

	void events() override;
};