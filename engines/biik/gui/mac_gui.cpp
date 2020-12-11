/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "biik/gui/mac_gui.h"
#include "biik/gui/cursors.h"
#include "biik/biik.h"
#include "biik/detection.h"

#include "common/macresman.h"
#include "engines/dialogs.h"
#include "engines/util.h"

#include "graphics/screen.h"
#include "graphics/macgui/macwindowmanager.h"
#include "graphics/macgui/macmenu.h"

#include "image/rosprite.h"

namespace Biik {

#define ACTION(menuID, item) ((menuID << 16) + item)
enum BiikMenuAction {
	kMenuAbout =   ACTION(1000, 0),
	kMenuOpen =    ACTION(1001, 0),
	kMenuSave =    ACTION(1001, 1),
	kMenuSound =   ACTION(1001, 3),
	kMenuRestart = ACTION(1001, 5),
	kMenuQuit =    ACTION(1001, 7)
};

MacGUI::MacGUI(BiikGame *vm)
	: BiikGUI(vm),
	  _macResMan(nullptr),
	  _wm(nullptr),
	  _menu(nullptr) {
}

MacGUI::~MacGUI() {
	delete _wm;
	delete _macResMan;
}

void menuCommandsCallback(int action, Common::String &text, void *data) {
	MacGUI *g = (MacGUI *)data;
	g->executeMenuCommand(action, text);
}

Common::Error MacGUI::init() {
	const char *exeName = _vm->getFileName(GAME_EXEFILE);
	_macResMan = new Common::MacResManager();
	if (exeName && !_macResMan->open(exeName))
		return Common::kNoGameDataFoundError;

	initGraphics(640, 480);
	_screen = new Graphics::Screen();
	_screen->setPalette(Image::riscos_palette_256, 0, 256);

	_wm = new Graphics::MacWindowManager();
	_wm->setScreen(_screen);
	_wm->passPalette(Image::riscos_palette_256, 256);

	_cursorMan = new MacCursorManager(_wm, _macResMan);

	_menu = _wm->addMenu();
	_menu->loadMenuBarResource(_macResMan, 128);
	_menu->setCommandsCallback(menuCommandsCallback, this);
	_menu->calcDimensions();

	return Common::kNoError;
}

void MacGUI::updateScreen() {
	_wm->draw();
	_screen->update();
}

bool MacGUI::processEvent(Common::Event &event) {
	return _wm->processEvent(event);
}

void MacGUI::executeMenuCommand(int action, Common::String &text) {
	switch((BiikMenuAction)action) {
	case kMenuOpen:
		_vm->loadGameDialog();
		break;
	case kMenuSave:
		_vm->saveGameDialog();
		break;
	case kMenuSound:
		{
			GUI::ConfigDialog dlg;
			dlg.runModal();
		}
		break;
	case kMenuQuit:
		_quit = true;
		break;
	case kMenuAbout:
	case kMenuRestart:
	default:
		warning("Menu option '%s' is not currently implemented", text.c_str());
	}
}

}
