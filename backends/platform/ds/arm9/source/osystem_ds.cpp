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

// Allow use of stuff in <time.h>
#define FORBIDDEN_SYMBOL_EXCEPTION_time_h

#include <nds.h>
#include <time.h>

#include "backends/platform/ds/arm9/source/osystem_ds.h"

#include "backends/audiocd/default/default-audiocd.h"
#include "backends/events/default/default-events.h"
#include "backends/fs/posix/posix-fs-factory.h"
#include "backends/graphics/ds/ds-graphics.h"
#include "backends/keymapper/hardware-input.h"
#include "backends/mutex/null/null-mutex.h"
#include "backends/saves/default/default-saves.h"
#include "backends/timer/default/default-timer.h"

#include "audio/mixer_intern.h"
#include "common/config-manager.h"
#include "common/translation.h"

OSystem_DS::OSystem_DS() : _rescanKeys(true) {
	_fsFactory = new POSIXFilesystemFactory();
}

OSystem_DS::~OSystem_DS() {
}

void OSystem_DS::initBackend() {
	ConfMan.setInt("autosave_period", 0);
	ConfMan.setBool("FM_medium_quality", true);

	_mutexManager = new NullMutexManager();
	_timerManager = new DefaultTimerManager();
	_eventManager = new DefaultEventManager(this);
	_savefileManager = new DefaultSaveFileManager();
	_graphicsManager = new DS::DSGraphicsManager();

	_mixer = new Audio::MixerImpl(22050);
	((Audio::MixerImpl *)_mixer)->setReady(false);

	_audiocdManager = new DefaultAudioCDManager();

	ModularBackend::initBackend();
}

static const Common::HardwareInputTableEntry ndsJoystickButtons[] = {
    { "JOY_A",              Common::JOYSTICK_BUTTON_A,              _s("A")           },
    { "JOY_B",              Common::JOYSTICK_BUTTON_B,              _s("B")           },
    { "JOY_X",              Common::JOYSTICK_BUTTON_X,              _s("X")           },
    { "JOY_Y",              Common::JOYSTICK_BUTTON_Y,              _s("Y")           },
    { "JOY_BACK",           Common::JOYSTICK_BUTTON_BACK,           _s("Select")      },
    { "JOY_START",          Common::JOYSTICK_BUTTON_START,          _s("Start")       },
    { "JOY_LEFT_SHOULDER",  Common::JOYSTICK_BUTTON_LEFT_SHOULDER,  _s("L")           },
    { "JOY_RIGHT_SHOULDER", Common::JOYSTICK_BUTTON_RIGHT_SHOULDER, _s("R")           },
    { "JOY_UP",             Common::JOYSTICK_BUTTON_DPAD_UP,        _s("D-pad Up")    },
    { "JOY_DOWN",           Common::JOYSTICK_BUTTON_DPAD_DOWN,      _s("D-pad Down")  },
    { "JOY_LEFT",           Common::JOYSTICK_BUTTON_DPAD_LEFT,      _s("D-pad Left")  },
    { "JOY_RIGHT",          Common::JOYSTICK_BUTTON_DPAD_RIGHT,     _s("D-pad Right") },
    { nullptr,              0,                                      nullptr           }
};

static const Common::AxisTableEntry ndsJoystickAxes[] = {
    { nullptr, 0, Common::kAxisTypeFull, nullptr }
};

Common::HardwareInputSet *OSystem_DS::getHardwareInputSet() {
	using namespace Common;

	CompositeHardwareInputSet *inputSet = new CompositeHardwareInputSet();
	inputSet->addHardwareInputSet(new JoystickHardwareInputSet(ndsJoystickButtons, ndsJoystickAxes));

	return inputSet;
}

void OSystem_DS::doJoyEvent(u32 keysPressed, u32 keysReleased, u32 ndsKey, uint8 svmButton) {
	if (keysPressed & ndsKey || keysReleased & ndsKey) {
		Common::Event event;
		event.type = (keysPressed & ndsKey) ? Common::EVENT_JOYBUTTON_DOWN : Common::EVENT_JOYBUTTON_UP;
		event.joystick.button = svmButton;

		_eventQueue.push(event);
	}
}

bool OSystem_DS::pollEvent(Common::Event &event) {
	if (!_rescanKeys) {
		if (!_eventQueue.empty()) {
			event = _eventQueue.pop();
			return true;
		} else {
			_rescanKeys = true;
			return false;
		}
	}

	scanKeys();
	_rescanKeys = false;

	int held = keysHeld();
	int keysPressed = keysDown();
	int keysReleased = keysUp();

	// Touch screen events
	if (held & KEY_TOUCH) {
		touchPosition touch;
		touchRead(&touch);

		event.type = Common::EVENT_MOUSEMOVE;
		event.mouse.x = touch.px;
		event.mouse.y = touch.py;

		_eventQueue.push(event);
	}

	// Button events
	doJoyEvent(keysPressed, keysReleased, KEY_A,      Common::JOYSTICK_BUTTON_A);
	doJoyEvent(keysPressed, keysReleased, KEY_B,      Common::JOYSTICK_BUTTON_B);
	doJoyEvent(keysPressed, keysReleased, KEY_SELECT, Common::JOYSTICK_BUTTON_BACK);
	doJoyEvent(keysPressed, keysReleased, KEY_START,  Common::JOYSTICK_BUTTON_START);
	doJoyEvent(keysPressed, keysReleased, KEY_RIGHT,  Common::JOYSTICK_BUTTON_DPAD_RIGHT);
	doJoyEvent(keysPressed, keysReleased, KEY_LEFT,   Common::JOYSTICK_BUTTON_DPAD_LEFT);
	doJoyEvent(keysPressed, keysReleased, KEY_UP,     Common::JOYSTICK_BUTTON_DPAD_UP);
	doJoyEvent(keysPressed, keysReleased, KEY_DOWN,   Common::JOYSTICK_BUTTON_DPAD_DOWN);
	doJoyEvent(keysPressed, keysReleased, KEY_R,      Common::JOYSTICK_BUTTON_RIGHT_SHOULDER);
	doJoyEvent(keysPressed, keysReleased, KEY_L,      Common::JOYSTICK_BUTTON_LEFT_SHOULDER);
	doJoyEvent(keysPressed, keysReleased, KEY_X,      Common::JOYSTICK_BUTTON_X);
	doJoyEvent(keysPressed, keysReleased, KEY_Y,      Common::JOYSTICK_BUTTON_Y);

	if (!_eventQueue.empty()) {
		event = _eventQueue.pop();
		return true;
	}

	return false;
}

uint32 OSystem_DS::getMillis(bool skipRecord) {
	return 0;
}

void OSystem_DS::delayMillis(uint msecs) {
}


void OSystem_DS::getTimeAndDate(TimeDate &td) const {
	time_t curTime = time(0);
	struct tm t = *localtime(&curTime);
	td.tm_sec = t.tm_sec;
	td.tm_min = t.tm_min;
	td.tm_hour = t.tm_hour;
	td.tm_mday = t.tm_mday;
	td.tm_mon = t.tm_mon;
	td.tm_year = t.tm_year;
	td.tm_wday = t.tm_wday;
}

void OSystem_DS::quit() {
}

void OSystem_DS::logMessage(LogMessageType::Type type, const char *message) {
	iprintf("%s", message);
}
