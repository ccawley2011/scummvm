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

#include "engines/advancedDetector.h"
#include "biik/biik.h"

namespace Biik {

const char *BiikGame::getGameId() const { return _gameDescription->gameId; }
Common::Platform BiikGame::getPlatform() const { return _gameDescription->platform; }
uint32 BiikGame::getFeatures() const { return _gameDescription->flags; }

const char *BiikGame::getFileName(int type) const {
	for (int i = 0; _gameDescription->filesDescriptions[i].fileType; i++) {
		if (_gameDescription->filesDescriptions[i].fileType == type)
			return _gameDescription->filesDescriptions[i].fileName;
	}
	return nullptr;
}

bool BiikGame::hasFeature(EngineFeature f) const {
	return (f == kSupportsReturnToLauncher); // TODO: Support loading and saving...
}

} // End of namespace Biik

class BiikMetaEngine : public AdvancedMetaEngine {
	const char *getName() const override {
		return "biik";
	}

	Common::Error createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const override;
	bool hasFeature(MetaEngineFeature f) const override;
};

Common::Error BiikMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	*engine = new Biik::BiikGame(syst, desc);
	return Common::kNoError;
}

bool BiikMetaEngine::hasFeature(MetaEngineFeature f) const {
	return false; // TODO: Support loading and saving...
}

#if PLUGIN_ENABLED_DYNAMIC(BIIK)
	REGISTER_PLUGIN_DYNAMIC(BIIK, PLUGIN_TYPE_ENGINE, BiikMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(BIIK, PLUGIN_TYPE_ENGINE, BiikMetaEngine);
#endif
