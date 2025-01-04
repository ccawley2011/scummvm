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

#include "mohawk/cstime_console.h"
#include "mohawk/cstime.h"
#include "mohawk/cursors.h"
#include "mohawk/resource.h"
#include "mohawk/sound.h"
#include "mohawk/video.h"

#include "common/system.h"
#include "common/textconsole.h"

namespace Mohawk {

CSTimeConsole::CSTimeConsole(MohawkEngine_CSTime *vm) : GUI::Debugger(), _vm(vm) {
	registerCmd("playSound",			WRAP_METHOD(CSTimeConsole, Cmd_PlaySound));
	registerCmd("stopSound",			WRAP_METHOD(CSTimeConsole, Cmd_StopSound));
	registerCmd("drawImage",			WRAP_METHOD(CSTimeConsole, Cmd_DrawImage));
	registerCmd("drawSubimage",			WRAP_METHOD(CSTimeConsole, Cmd_DrawSubimage));
	registerCmd("changeCase",			WRAP_METHOD(CSTimeConsole, Cmd_ChangeCase));
	registerCmd("changeScene",			WRAP_METHOD(CSTimeConsole, Cmd_ChangeScene));
	registerCmd("caseVariable",			WRAP_METHOD(CSTimeConsole, Cmd_CaseVariable));
	registerCmd("invItem",			WRAP_METHOD(CSTimeConsole, Cmd_InvItem));
}

CSTimeConsole::~CSTimeConsole() {
}

bool CSTimeConsole::Cmd_PlaySound(int argc, const char **argv) {
	if (argc == 1) {
		debugPrintf("Usage: playSound <value>\n");
		return true;
	}

	_vm->_sound->stopSound();
	_vm->_sound->playSound((uint16)atoi(argv[1]));
	return false;
}

bool CSTimeConsole::Cmd_StopSound(int argc, const char **argv) {
	debugPrintf("Stopping Sound\n");

	_vm->_sound->stopSound();
	return true;
}

bool CSTimeConsole::Cmd_DrawImage(int argc, const char **argv) {
	if (argc == 1) {
		debugPrintf("Usage: drawImage <value>\n");
		return true;
	}

	_vm->_gfx->copyAnimImageToScreen((uint16)atoi(argv[1]));
	_vm->_system->updateScreen();
	return false;
}

bool CSTimeConsole::Cmd_DrawSubimage(int argc, const char **argv) {
	if (argc < 3) {
		debugPrintf("Usage: drawSubimage <value> <subimage>\n");
		return true;
	}

	_vm->_gfx->copyAnimSubImageToScreen((uint16)atoi(argv[1]), (uint16)atoi(argv[2]));
	_vm->_system->updateScreen();
	return false;
}

bool CSTimeConsole::Cmd_ChangeCase(int argc, const char **argv) {
	if (argc < 2) {
		debugPrintf("Usage: changeCase <value>\n");
		return true;
	}

	error("Can't change case yet"); // FIXME
	return false;
}

bool CSTimeConsole::Cmd_ChangeScene(int argc, const char **argv) {
	if (argc < 2) {
		debugPrintf("Usage: changeScene <value>\n");
		return true;
	}

	_vm->addEvent(CSTimeEvent(kCSTimeEventNewScene, 0xffff, atoi(argv[1])));
	return false;
}

bool CSTimeConsole::Cmd_CaseVariable(int argc, const char **argv) {
	if (argc < 2) {
		debugPrintf("Usage: caseVariable <id> [<value>]\n");
		return true;
	}

	if (argc == 2) {
		debugPrintf("case variable %d has value %d\n", atoi(argv[1]), _vm->_caseVariable[atoi(argv[1])]);
	} else {
		_vm->_caseVariable[atoi(argv[1])] = atoi(argv[2]);
	}
	return true;
}

bool CSTimeConsole::Cmd_InvItem(int argc, const char **argv) {
	if (argc < 3) {
		debugPrintf("Usage: invItem <id> <0 or 1>\n");
		return true;
	}

	if (atoi(argv[2])) {
		_vm->addEvent(CSTimeEvent(kCSTimeEventDropItemInInventory, 0xffff, atoi(argv[1])));
	} else {
		_vm->addEvent(CSTimeEvent(kCSTimeEventRemoveItemFromInventory, 0xffff, atoi(argv[1])));
	}
	return false;
}

} // End of namespace Mohawk
