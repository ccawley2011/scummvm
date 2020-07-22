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

#include "biik/biik.h"
#include "biik/archive.h"
#include "biik/console.h"
#include "biik/script.h"
#include "biik/sound.h"

#include "biik/gui/riscos_gui.h"
#include "biik/gui/windows_gui.h"
#include "biik/gui/mac_gui.h"

#include "common/config-manager.h"
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/fs.h"
#include "common/system.h"

#include "engines/util.h"


namespace Biik {

BiikGame::BiikGame(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst), _gameDescription(gameDesc) {
	_gui = nullptr;
	_archive = nullptr;
	_script = nullptr;

	DebugMan.addDebugChannel(kDebugGeneral, "general", "General debug level");
	DebugMan.addDebugChannel(kDebugGUI, "gui", "GUI debug level");
}

BiikGame::~BiikGame() {
	delete _script;
	delete _archive;
	delete _gui;
}

static const struct {
	const char *gameId;
	Common::Platform platform;
	const char *subdirectory;
	bool isWaveDir;
} searchPaths[] = {
	{ "betsi", Common::kPlatformWindows, "BETSI", true },
	{ "betsi", Common::kPlatformWindows, "Audio", true },
	{ "betsi", Common::kPlatformMacintosh, "data", false },
	{ "guardians", Common::kPlatformAcorn, "!Guardians", false },
	{ "guardians", Common::kPlatformAcorn, "!Guardians/mode21", false },
	{ "puppy", Common::kPlatformWindows, "Puppy", true },
	{ "puppy", Common::kPlatformWindows, "AUDIO", true },
	{ "puppy", Common::kPlatformMacintosh, "data", false },
	{ "puppy", Common::kPlatformMacintosh, "AUDIO", true },
};

Common::Error BiikGame::run() {
	setDebugger(new Console(this));

	const Common::FSNode gameDataDir(ConfMan.get("path"));
	Common::FSNode waveDir(gameDataDir);
	for (int i = 0; i < ARRAYSIZE(searchPaths); i++) {
		if (strcmp(searchPaths[i].gameId, getGameId()) != 0)
			continue;

		if (searchPaths[i].platform != getPlatform())
			continue;

		SearchMan.addSubDirectoryMatching(gameDataDir, searchPaths[i].subdirectory);

		if (searchPaths[i].isWaveDir)
			waveDir = waveDir.getChild(searchPaths[i].subdirectory);
	}

	switch (getPlatform()) {
	case Common::kPlatformAcorn:
		_gui = new RISCOSGUI(this);
		break;
	case Common::kPlatformWindows:
		_gui = new WindowsGUI(this);
		break;
	case Common::kPlatformMacintosh:
		_gui = new MacGUI(this);
		break;
	default:
		error("Unrecognised platform");
	}

	Common::Error guiError = _gui->init();
	if (guiError.getCode() != Common::kNoError)
		return guiError;

	_sound = new Sound(this, waveDir);

	_archive = new BiikArchive(isBigEndian());
	if (!_archive->open(getFileName(GAME_STARTFILE)))
		return Common::kNoGameDataFoundError;

	_script = new Script(this);
	_script->load(_archive, "Text");

	return _gui->run();
}

} // End of namespace Biik
