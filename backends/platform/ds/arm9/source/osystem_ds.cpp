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
#include "backends/mutex/null/null-mutex.h"
#include "backends/saves/default/default-saves.h"
#include "backends/timer/default/default-timer.h"

#include "audio/mixer_intern.h"
#include "common/config-manager.h"

OSystem_DS::OSystem_DS() {
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

bool OSystem_DS::pollEvent(Common::Event &event) {
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
