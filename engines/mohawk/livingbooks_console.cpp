/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "mohawk/livingbooks_console.h"
#include "mohawk/cursors.h"
#include "mohawk/livingbooks.h"
#include "mohawk/resource.h"
#include "mohawk/sound.h"
#include "mohawk/video.h"

#include "common/system.h"
#include "common/textconsole.h"

namespace Mohawk {

LivingBooksConsole::LivingBooksConsole(MohawkEngine_LivingBooks *vm) : GUI::Debugger(), _vm(vm) {
	registerCmd("playSound",			WRAP_METHOD(LivingBooksConsole, Cmd_PlaySound));
	registerCmd("stopSound",			WRAP_METHOD(LivingBooksConsole, Cmd_StopSound));
	registerCmd("drawImage",			WRAP_METHOD(LivingBooksConsole, Cmd_DrawImage));
	registerCmd("changePage",			WRAP_METHOD(LivingBooksConsole, Cmd_ChangePage));
	registerCmd("changeCursor",			WRAP_METHOD(LivingBooksConsole, Cmd_ChangeCursor));
}

LivingBooksConsole::~LivingBooksConsole() {
}

bool LivingBooksConsole::Cmd_PlaySound(int argc, const char **argv) {
	if (argc == 1) {
		debugPrintf("Usage: playSound <value>\n");
		return true;
	}

	_vm->_sound->stopSound();
	_vm->_sound->playSound((uint16)atoi(argv[1]));
	return false;
}

bool LivingBooksConsole::Cmd_StopSound(int argc, const char **argv) {
	debugPrintf("Stopping Sound\n");

	_vm->_sound->stopSound();
	return true;
}

bool LivingBooksConsole::Cmd_DrawImage(int argc, const char **argv) {
	if (argc == 1) {
		debugPrintf("Usage: drawImage <value>\n");
		return true;
	}

	_vm->_gfx->copyAnimImageToScreen((uint16)atoi(argv[1]));
	_vm->_system->updateScreen();
	return false;
}

bool LivingBooksConsole::Cmd_ChangePage(int argc, const char **argv) {
	if (argc < 2 || argc > 3) {
		debugPrintf("Usage: changePage <page>[.<subpage>] [<mode>]\n");
		return true;
	}

	int page, subpage = 0;
	if (sscanf(argv[1], "%d.%d", &page, &subpage) == 0) {
		debugPrintf("Usage: changePage <page>[.<subpage>] [<mode>]\n");
		return true;
	}
	LBMode mode = argc == 2 ? _vm->getCurMode() : (LBMode)atoi(argv[2]);
	if (subpage == 0) {
		if (_vm->tryLoadPageStart(mode, page))
			return false;
	} else {
		if (_vm->loadPage(mode, page, subpage))
			return false;
	}
	debugPrintf("no such page %d.%d\n", page, subpage);
	return true;
}

bool LivingBooksConsole::Cmd_ChangeCursor(int argc, const char **argv) {
	if (argc == 1) {
		debugPrintf("Usage: changeCursor <value>\n");
		return true;
	}

	_vm->_cursor->setCursor((uint16)atoi(argv[1]));
	return true;
}

} // End of namespace Mohawk
