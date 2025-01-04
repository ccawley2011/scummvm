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

#include "mohawk/myst_console.h"
#include "mohawk/cursors.h"
#include "mohawk/resource.h"
#include "mohawk/sound.h"
#include "mohawk/video.h"

#include "common/system.h"
#include "common/textconsole.h"

#include "mohawk/myst.h"
#include "mohawk/myst_areas.h"
#include "mohawk/myst_card.h"
#include "mohawk/myst_graphics.h"
#include "mohawk/myst_scripts.h"
#include "mohawk/myst_sound.h"

namespace Mohawk {

MystConsole::MystConsole(MohawkEngine_Myst *vm) : GUI::Debugger(), _vm(vm) {
	registerCmd("changeCard",			WRAP_METHOD(MystConsole, Cmd_ChangeCard));
	registerCmd("curCard",			WRAP_METHOD(MystConsole, Cmd_CurCard));
	registerCmd("var",				WRAP_METHOD(MystConsole, Cmd_Var));
	registerCmd("curStack",			WRAP_METHOD(MystConsole, Cmd_CurStack));
	registerCmd("changeStack",		WRAP_METHOD(MystConsole, Cmd_ChangeStack));
	registerCmd("drawImage",			WRAP_METHOD(MystConsole, Cmd_DrawImage));
	registerCmd("drawRect",			WRAP_METHOD(MystConsole, Cmd_DrawRect));
	registerCmd("setResourceEnable",	WRAP_METHOD(MystConsole, Cmd_SetResourceEnable));
	registerCmd("playSound",			WRAP_METHOD(MystConsole, Cmd_PlaySound));
	registerCmd("stopSound",			WRAP_METHOD(MystConsole, Cmd_StopSound));
	registerCmd("playMovie",			WRAP_METHOD(MystConsole, Cmd_PlayMovie));
	registerCmd("disableInitOpcodes",	WRAP_METHOD(MystConsole, Cmd_DisableInitOpcodes));
	registerCmd("cache",				WRAP_METHOD(MystConsole, Cmd_Cache));
	registerCmd("resources",			WRAP_METHOD(MystConsole, Cmd_Resources));
	registerCmd("quickTest",            WRAP_METHOD(MystConsole, Cmd_QuickTest));
	registerVar("show_resource_rects",  &_vm->_showResourceRects);
}

MystConsole::~MystConsole() {
}

bool MystConsole::Cmd_ChangeCard(int argc, const char **argv) {
	if (argc < 2) {
		debugPrintf("Usage: changeCard <card>\n");
		return true;
	}

	_vm->_sound->stopEffect();
	_vm->changeToCard((uint16)atoi(argv[1]), kTransitionCopy);

	return false;
}

bool MystConsole::Cmd_CurCard(int argc, const char **argv) {
	debugPrintf("Current Card: %d\n", _vm->getCard()->getId());
	return true;
}

bool MystConsole::Cmd_Var(int argc, const char **argv) {
	if (argc == 1) {
		debugPrintf("Usage: var <var> (<value>)\n");
		return true;
	}

	if (argc > 2)
		_vm->_stack->setVarValue((uint16)atoi(argv[1]), (uint16)atoi(argv[2]));

	debugPrintf("%d = %d\n", (uint16)atoi(argv[1]), _vm->_stack->getVar((uint16)atoi(argv[1])));

	return true;
}

static const char *mystStackNames[12] = {
	"Channelwood",
	"Credits",
	"Demo",
	"D'ni",
	"Intro",
	"MakingOf",
	"Mechanical",
	"Myst",
	"Selenitic",
	"Slideshow",
	"SneakPreview",
	"Stoneship"
};

static const uint16 default_start_card[12] = {
	3137,
	10000,
	2000,
	5038,
	1,
	1,
	6122,
	4134,
	1282,
	1000,
	3000,
	2029
};

bool MystConsole::Cmd_CurStack(int argc, const char **argv) {
	debugPrintf("Current Stack: %s\n", mystStackNames[_vm->_stack->getStackId()]);
	return true;
}

bool MystConsole::Cmd_ChangeStack(int argc, const char **argv) {
	if (argc != 2 && argc != 3) {
		debugPrintf("Usage: changeStack <stack> [<card>]\n\n");
		debugPrintf("Stacks:\n=======\n");

		for (byte i = 0; i < ARRAYSIZE(mystStackNames); i++)
			debugPrintf(" %s\n", mystStackNames[i]);

		debugPrintf("\n");

		return true;
	}

	byte stackNum = 0;

	for (byte i = 1; i <= ARRAYSIZE(mystStackNames); i++)
		if (!scumm_stricmp(argv[1], mystStackNames[i - 1])) {
			stackNum = i;
			break;
		}

	if (!stackNum) {
		debugPrintf("\'%s\' is not a stack name!\n", argv[1]);
		return true;
	}

	// We need to stop any playing sound when we change the stack
	// as the next card could continue playing it if it.
	_vm->_sound->stopEffect();

	uint16 card = 0;
	if (argc == 3)
		card = (uint16)atoi(argv[2]);
	else
		card = default_start_card[stackNum - 1];

	_vm->changeToStack(static_cast<MystStack>(stackNum - 1), card, 0, 0);

	return false;
}

bool MystConsole::Cmd_DrawImage(int argc, const char **argv) {
	if (argc != 2 && argc != 6) {
		debugPrintf("Usage: drawImage <image> [<left> <top> <right> <bottom>]\n");
		return true;
	}

	Common::Rect rect;

	if (argc == 2)
		rect = Common::Rect(0, 0, 544, 333);
	else
		rect = Common::Rect((uint16)atoi(argv[2]), (uint16)atoi(argv[3]), (uint16)atoi(argv[4]), (uint16)atoi(argv[5]));

	_vm->_gfx->copyImageToScreen((uint16)atoi(argv[1]), rect);
	return false;
}

bool MystConsole::Cmd_DrawRect(int argc, const char **argv) {
	if (argc != 5 && argc != 2) {
		debugPrintf("Usage: drawRect <left> <top> <right> <bottom>\n");
		debugPrintf("Usage: drawRect <resource id>\n");
		return true;
	}

	if (argc == 5) {
		_vm->_gfx->drawRect(Common::Rect((uint16)atoi(argv[1]), (uint16)atoi(argv[2]), (uint16)atoi(argv[3]), (uint16)atoi(argv[4])), kRectEnabled);
	} else if (argc == 2) {
		uint16 resourceId = (uint16)atoi(argv[1]);
		if (resourceId < _vm->getCard()->_resources.size())
			_vm->getCard()->_resources[resourceId]->drawBoundingRect();
	}

	return false;
}

bool MystConsole::Cmd_SetResourceEnable(int argc, const char **argv) {
	if (argc < 3) {
		debugPrintf("Usage: setResourceEnable <resource id> <bool>\n");
		return true;
	}

	_vm->getCard()->setResourceEnabled((uint16)atoi(argv[1]), atoi(argv[2]) == 1);
	return true;
}

bool MystConsole::Cmd_PlaySound(int argc, const char **argv) {
	if (argc == 1) {
		debugPrintf("Usage: playSound <value>\n");

		return true;
	}

	_vm->_sound->playEffect((uint16) atoi(argv[1]));

	return false;
}

bool MystConsole::Cmd_StopSound(int argc, const char **argv) {
	debugPrintf("Stopping Sound\n");

	_vm->_sound->stopEffect();

	return true;
}

bool MystConsole::Cmd_PlayMovie(int argc, const char **argv) {
	if (argc < 3) {
		debugPrintf("Usage: playMovie <name> <stack> [<left> <top>]\n");
		debugPrintf("NOTE: The movie will play *once* in the background.\n");
		return true;
	}

	Common::String fileName = argv[1];
	int8 stackNum = -1;
	for (byte i = 0; i < ARRAYSIZE(mystStackNames); i++)
		if (!scumm_stricmp(argv[2], mystStackNames[i])) {
			stackNum = i;
			break;
		}

	if (stackNum < 0) {
		debugPrintf("\'%s\' is not a stack name!\n", argv[2]);
		return true;
	}

	VideoEntryPtr video = _vm->playMovie(fileName, static_cast<MystStack>(stackNum));

	if (argc == 4) {
		video->setX(atoi(argv[2]));
		video->setY(atoi(argv[3]));
	} else if (argc > 4) {
		video->setX(atoi(argv[3]));
		video->setY(atoi(argv[4]));
	} else {
		video->center();
	}

	return false;
}

bool MystConsole::Cmd_DisableInitOpcodes(int argc, const char **argv) {
	if (argc != 1) {
		debugPrintf("Usage: disableInitOpcodes\n");

		return true;
	}

	_vm->_stack->disablePersistentScripts();

	return true;
}

bool MystConsole::Cmd_Cache(int argc, const char **argv) {
	if (argc > 2) {
		debugPrintf("Usage: cache on/off - Omit parameter to get current state\n");
		return true;
	}

	bool state = false;

	if (argc == 1) {
		state = _vm->getCacheState();
	} else {
		if (!scumm_stricmp(argv[1], "on"))
			state = true;

		_vm->setCacheState(state);
	}

	debugPrintf("Cache: %s\n", state ? "Enabled" : "Disabled");
	return true;
}

bool MystConsole::Cmd_Resources(int argc, const char **argv) {
	debugPrintf("Resources in card %d:\n", _vm->getCard()->getId());

	for (uint i = 0; i < _vm->getCard()->_resources.size(); i++) {
		debugPrintf("#%2d %s\n", i, _vm->getCard()->_resources[i]->describe().c_str());
	}

	return true;
}

bool MystConsole::Cmd_QuickTest(int argc, const char **argv) {
	_debugPauseToken.clear();

	// Go through all the ages, all the views and click random stuff
	for (uint i = 0; i < ARRAYSIZE(mystStackNames); i++) {
		MystStack stackId = static_cast<MystStack>(i);
		if (stackId == kDemoStack || stackId == kMakingOfStack
		    || stackId == kDemoSlidesStack || stackId == kDemoPreviewStack) continue;

		debug("Loading stack %s", mystStackNames[stackId]);
		_vm->changeToStack(stackId, default_start_card[stackId], 0, 0);

		Common::Array<uint16> ids = _vm->getResourceIDList(ID_VIEW);
		for (uint j = 0; j < ids.size(); j++) {
			if (ids[j] == 4632) continue;

			debug("Loading card %d", ids[j]);
			_vm->changeToCard(ids[j], kTransitionCopy);

			_vm->doFrame();

			{
				MystCardPtr card = _vm->getCardPtr();
				int16 resIndex = _vm->_rnd->getRandomNumber(card->_resources.size()) - 1;
				if (resIndex >= 0 && _vm->getCard()->_resources[resIndex]->isEnabled()) {
					card->_resources[resIndex]->handleMouseDown();
					card->_resources[resIndex]->handleMouseUp();
				}
			}

			_vm->doFrame();

			if (_vm->_stack->getStackId() != stackId) {
				// Clicking may have linked us to another age
				_vm->changeToStack(stackId, default_start_card[stackId], 0, 0);
			}
		}
	}

	_debugPauseToken = _vm->pauseEngine();
	return true;
}

} // End of namespace Mohawk
